/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
 * [2] http://www.3dkingdoms.com/weekly/weekly.php?a=2
 * [3] https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
 * [4] https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
 */
#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <thread>

#include "ImageFile.h"
#include "Renderer.h"
#include "Utility.h"
#include "Vector.h"


Renderer::Renderer(
    Scene &scene,
    int width,
    int height,
    int maxDepth,
    int antiAliasing,
    AntiAliasingMethod antiAliasMethod,
    bool enableSoftShadows,
    int noiseReduction,
    int numThreads
)
: mScene(scene)
, mWidth(width)
, mHeight(height)
, mNoiseReduction(noiseReduction)
, mMaxDepth(maxDepth)
, mAntiAliasing(antiAliasing)
, mAntiAliasingMethod(antiAliasMethod)
, mEnableSoftShadows(enableSoftShadows)
, mNumThreads(numThreads)
{
    int s = (int) sqrtf(mAntiAliasing);
    if (s * s != antiAliasing) {
        printf("Warning! Anti-aliasing sample quantity must be a square number.\n");
    }
}


void Renderer::render() {
    Vec3f *image = new Vec3f[mHeight * mWidth];
    float aspectRatio = (float) mWidth / (float) mHeight;
    float fovRatio = tan(mScene.mCamera.mFieldOfViewRadians / 2.0f);

    std::vector<std::shared_ptr<RenderThread>> threads;
    for (int i = 0; i < mNumThreads; i++) {
        auto t = std::make_shared<RenderThread>(this, aspectRatio, fovRatio);
        threads.push_back(t);

        t->run(image, i, mNumThreads);
    }

    for (auto t : threads) {
        t->join();
    }

    writeImage("./Ray.ppm", mWidth, mHeight, image);
    delete [] image;
}


RenderThread::RenderThread(Renderer *renderer, float aspectRatio, float fovRatio)
: mRenderer(renderer)
, mStats()
, mAspectRatio(aspectRatio)
, mFovRatio(fovRatio)
, mThread(NULL)
{}


void RenderThread::join() {
    mThread->join();
    mStats.print();
}


/**
 * Computes the direction of a ray from the focal point to the (x, y)
 * pixel space coordinate on the rendering plane (the final image!).
 *
 * \param aspectRatio \f$\frac{\text{image width}}{\text{image height}}\f$
 * \param fovRatio This should be computed by
 *      \f$\tan\left(\frac{\text{fov}}{2}\right)\f$ where fov is the vertical
 *      field of view in radians. It is halved because the ray direction is
 *      computed with a right triangle formed from splitting the field of view
 *      in two halves.
 * \param x Pixel space coordinate on the rendering plane.
 * \param y Pixel space coordinate on the rendering plane.
 */
Vec3f RenderThread::computePrimaryRay(int x, int y, float xS, float yS) {
    // Loosely based on [1].
    //
    // Normalize the raster space (mWidth by mHeight pixels) into
    // [0,1] with a 0.5 shift in raster space to center the pixels.
    // NDC assumes (0, 0) is the top-left point.
    float deviceCoordX = (x + xS) / (float) mRenderer->mWidth;
    float deviceCoordY = (y + yS) / (float) mRenderer->mHeight;
    // Centered (0, 0) origin with (1, 1) in the top-right.
    float pixelX = (2 * deviceCoordX - 1) * mFovRatio * mAspectRatio;
    float pixelY = (1 - 2 * deviceCoordY) * mFovRatio;

    return normalize(Vec3f({ pixelX, pixelY, -1 }));
}


/**
 * std::thread can't accept an instance method so we need this wrapper.
 */
void threadBody(RenderThread *t, Vec3f *image, const int startX, const int step) {
    t->render(image, startX, step);
}


void RenderThread::run(Vec3f *image, const int startX, const int step) {
    mThread = std::make_shared<std::thread>(threadBody, this, image, startX, step);
}


/**
 * Render a single pixel multiple times and return the average colour.
 */
Vec3f RenderThread::computePixelAverage(int x, int y) {
    Vec3f color({ 0, 0, 0 });
    for (int i = 0; i < mRenderer->mNoiseReduction; i++) {
        color = add(color, renderPixel(x, y));
    }

    return divide(color, (float) mRenderer->mNoiseReduction);
}


/**
 * Each thread is ``staggered'' across the image instead of each thread being
 * responsible for separate rows. I haven't done thorough testing on this but
 * my hypothesis is that this will ensure each thread does more equal amounts
 * of work, since the threads will have similar distributions of the image.
 * If a scene did not have much going on in its top half, then assigning the
 * top rows to some threads exclusively would waste potential concurrency.
 *
 * That is, if there are two threads A and B then the distribution of pixels
 * each renders on a 3x6 image will look like this:
 *
 *     ABABAB
 *     ABABAB
 *     ABABAB
 */
void RenderThread::render(Vec3f *image, const int startX, const int step) {
    TimePoint startTime = Clock::now();
    Vec3f *p = image + startX;
    mStats.pixels = 0;

    for (int y = 0; y < mRenderer->mHeight; y++) {
        for (int x = startX; x < mRenderer->mWidth; x+= step, p += step) {
            mStats.pixels++;
            *p = computePixelAverage(x, y);
        }
    }

    mStats.id = startX;
    mStats.timeSeconds = getSecondsSince(startTime);
}


void RenderThread::computeAntiAliasingSample(int samples, int x, int y, float &xS, float &yS) {
    if (mRenderer->mAntiAliasingMethod == REGULAR) {
        xS = x * (1.0f / samples) - 0.5f;
        yS = y * (1.0f / samples) - 0.5f;
    } else if (mRenderer->mAntiAliasingMethod == RANDOM) {
        xS = (rand() % 1000) / 1000.0f - 0.5f;
        yS = (rand() % 1000) / 1000.0f - 0.5f;
    }
}


Vec3f RenderThread::renderPixel(int x, int y) {
    if (mRenderer->mAntiAliasing == 0) {
        Vec3f ray = computePrimaryRay(x, y, 0.5f, 0.5f);
        mStats.quantities[PRIMARY]++;
        return trace(ray);
    }

    int s = (int) sqrtf(mRenderer->mAntiAliasing);
    Vec3f color({ 0, 0, 0 });
    for (int ySampling = 0; ySampling < s; ySampling++) {
        for (int xSampling = 0; xSampling < s; xSampling++) {
            float xS = 0;
            float yS = 0;
            computeAntiAliasingSample(s, xSampling, ySampling, xS, yS);
            Vec3f ray = computePrimaryRay(x, y, xS, yS);

            color = add(color, trace(ray));
            mStats.quantities[PRIMARY]++;
        }
    }

    return divide(color, (float) mRenderer->mAntiAliasing);
}


Vec3f RenderThread::trace(Vec3f ray) {
    return trace(Vec3f({0, 0, 0}), ray, 0);
}


Vec3f RenderThread::trace(Vec3f origin, Vec3f ray, int depth) {
    /*if (fabs(norm(ray) - 1) > 1e-4) {
        printf("norm: %f\n", norm(ray));
    }*/

    std::shared_ptr<SceneObject> intersectionObject = NULL;
    float intersectionScalar;
    bool doesIntersect = mRenderer->mScene.getIntersection(
        origin,
        ray,
        intersectionObject,
        intersectionScalar
    );

    if (!doesIntersect) {
        return Vec3f({ 0, 0, 0 });
    }
    mStats.quantities[INTERSECTIONS]++;

    Vec3f intersection = add(origin, multiply(ray, intersectionScalar));
    Vec3f normal = intersectionObject->getNormalDir(intersection);
    Vec3f color = multiply(
        intersectionObject->getColor(REST(intersection)),
        intersectionObject->mMaterial->ambient
    );
    float diffuse = intersectionObject->mMaterial->diffuse;

    if (diffuse > 0) {
        for (auto pointLight : mRenderer->mScene.mPointLights) {
            float distance;
            Vec3f shadowRay = pointLight->direction(
                intersection,
                distance,
                mRenderer->mEnableSoftShadows
            );
            mStats.quantities[SHADOW]++;

            float k;
            float intensity = 1;
            for (auto testObj : mRenderer->mScene.mObjects) {
                if (testObj == intersectionObject) {
                    continue;
                }
                // We need to check for just a smidge of bias to ensure we're not
                // intersecting with testObj. We also need to ensure that the
                // distance to the object is less than the distance to the light,
                // since the light could be between the two objects (especially
                // with planes where there is usually an intersection with the
                // ray).
                if (testObj->intersect(intersection, shadowRay, k) && k >= 1e-4 && k < distance) {
                    intensity -= 1 - fmax(0, testObj->mMaterial->transmission);
                    if (intensity <= 1e-4) {
                        break;
                    }
                }
            }

            color = add(
                color,
                multiply(
                    intersectionObject->getColor(REST(intersection)),
                    intensity * pointLight->mIntensity * diffuse * fmax(0, dot(shadowRay, normal))
                )
            );
        }
    }

    bool isTotalInternalReflection = false;
    if (intersectionObject->mMaterial->transmission > 0 && depth < mRenderer->mMaxDepth) {
        Vec3f transmissionDirection = computeRefractionDir(
            ray,
            normal,
            intersectionObject->mMaterial->refractiveIndex,
            isTotalInternalReflection
        );
        if (!isTotalInternalReflection) {
            // The surface normal points away from the sphere. We want the
            // bias to be in the direction of the transmission. If the
            // incoming ray is outside the sphere then the bias should be
            // negating the normal.
            Vec3f transmissionColor = trace(
                add(intersection, multiply(transmissionDirection, 1e-4)),
                transmissionDirection,
                depth + 1
            );
            color = add(
                color,
                multiply(transmissionColor, intersectionObject->mMaterial->transmission)
            );
            mStats.quantities[TRANSMISSION]++;
        }
    }

    float intensity = intersectionObject->mMaterial->specular;
    if (isTotalInternalReflection) {
        intensity += intersectionObject->mMaterial->transmission;
    }
    if (intensity > 0 && depth < mRenderer->mMaxDepth) {
        Vec3f reflectionDirection = computeReflectionDir(ray, normal);
        Vec3f reflectionColor = trace(
            add(intersection, multiply(reflectionDirection, 1e-5)),
            reflectionDirection,
            depth + 1
        );
        color = add(
            color,
            multiply(reflectionColor, intensity)
        );
        mStats.quantities[SPECULAR]++;
    }

    return truncate(color, 1);
}


bool isInside(Vec3f rayDirection, Vec3f intersectionNormal) {
    return dot(rayDirection, intersectionNormal) > 0;
}


// Based on [2] and [4].
Vec3f computeReflectionDir(Vec3f incomingRayDirection, Vec3f surfaceNormal) {
    // Projection theorem tells us that any vector is the sum of its
    // projections on to two orthogonal spaces.
    return subtract(
        incomingRayDirection,
        // Project the incoming ray onto the intersection surface normal.
        multiply(surfaceNormal, 2 * dot(incomingRayDirection, surfaceNormal))
    );
}


/**
 * Computes the direction of the transmission ray by computing the refraction
 * on the incident ray.
 *
 * Assumption: The interface is always between "air" and the medium of the
 * object the incident ray has intersected. This means that this function will
 * not correctly handle a situation where a ray is travelling through object A
 * and then intersects object B before leaving object A. TODO #2
 *
 * The transmission ray loses energy as it approaches the critical angle.
 * Once it is parallel or beyond the critical angle, all the energy is
 * exclusively in the incident ray.
 *
 * Based on [3].
 */
Vec3f computeRefractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection) {
    float relativeIndexOfRefraction;
    float cosi = -dot(ray, normal);
    if (isInside(ray, normal)) {
        relativeIndexOfRefraction = refractionIndex;
        cosi *= -1;
    } else {
        relativeIndexOfRefraction = 1.0f / refractionIndex;
        normal = multiply(normal, -1);
    }
    assert(cosi >= 0);

    float base = (
        1 - (relativeIndexOfRefraction * relativeIndexOfRefraction) * (1 - cosi * cosi)
    );
    if (base < 0) {
        isTotalInternalReflection = true;
        return ray;
    }

    return add(
        multiply(ray, relativeIndexOfRefraction),
        multiply(normal, sqrtf(base) - relativeIndexOfRefraction * cosi)
    );
}
