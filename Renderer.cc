#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "ImageFile.h"
#include "Renderer.h"
#include "Utility.h"
#include "Vector.h"


#define WORK_BLOCK_SIZE 4


Renderer::Renderer(Scene &scene)
: mWorkQueue()
, mQueueLock()
, mCompletedRows(0)
, mImage(NULL)
, mScene(scene)
, mWidth(600)
, mHeight(500)
, mNoiseReduction(1)
, mMaxDepth(3)
, mAntiAliasing(0)
, mAntiAliasingMethod(REGULAR)
, mEnableSoftShadows(false)
, mNumThreads(1)
, mOutputFile("./Ray.ppm")
{
    int s = (int) sqrtf(mAntiAliasing);
    if (s * s != mAntiAliasing) {
        printf("Warning! Anti-aliasing sample quantity must be a square number.\n");
    }
}


Renderer::~Renderer() {
    delete [] mImage;
}


/**
 * Pushes blocks of rows on to a queue and starts render threads which will pop
 * off the queue and render independent rows.
 */
void Renderer::render() {
    mImage = new Vec3f[mHeight * mWidth];
    // Compute properties shared by all primary ray computations in all threads.
    float aspectRatio = (float) mWidth / (float) mHeight;
    float fovRatio = tan(mScene.mCamera.mFieldOfViewRadians / 2.0f);

    // Divide the rows of the image into blocks and place them in the work
    // queue.
    mQueueLock.lock();
    for (
        int i = WORK_BLOCK_SIZE - 1, j = 0;
        j <= mHeight;
        j = i + 1, i += WORK_BLOCK_SIZE
    ) {
        mWorkQueue.push(std::make_pair(j, std::min(mHeight - 1, i)));
    }
    mQueueLock.unlock();

    std::vector<std::shared_ptr<RenderThread>> threads;
    for (int i = 0; i < mNumThreads; i++) {
        auto t = std::make_shared<RenderThread>(this, aspectRatio, fovRatio);
        threads.push_back(t);

        t->run(mImage, i);
    }
    for (auto t : threads) {
        t->join();
    }
    std::cout << std::endl << std::endl;
    for (auto t : threads) {
        t->mStats.print();
    }

    writeImage(mOutputFile, mWidth, mHeight, mImage);
}


void Renderer::gl() {
    renderImageToGL(mWidth, mHeight, mImage);
}


/**
 * Each render thread gets a batch of WORK_BLOCK_SIZE rows to render at a time.
 * This function returns true if there is work remaining in the queue after
 * it populates `start` and `end` (if the queue was non-empty).
 */
bool Renderer::getWork(int &start, int &end) {
    mQueueLock.lock();
    if (end != 0) {
        mCompletedRows += (end + 1 - start);
    }
    if (mWorkQueue.empty()) {
        printProgress();
        mQueueLock.unlock();
        return false;
    }

    auto t = mWorkQueue.front();
    mWorkQueue.pop();
    printProgress();
    mQueueLock.unlock();

    std::tie(start, end) = t;
    return true;
}


/// Just a simple progress bar using a carriage return to write over itself.
void Renderer::printProgress() {
    float progress = (mCompletedRows / (float) mHeight) * 100;
    std::cout << std::right
             << std::fixed << std::setw(5) << std::setprecision(1) << std::setfill(' ')
             << progress << "% [";
    for (int i = 0; i < 100; i++) {
        std::cout << (i < progress ? "=" : " ");
    }
    std::cout << "]\r";
    std::cout.flush();
}


RenderThread::RenderThread(Renderer *renderer, float aspectRatio, float fovRatio)
: mRenderer(renderer)
, mStats()
, mAspectRatio(aspectRatio)
, mFovRatio(fovRatio)
, mThread(NULL)
{}


void RenderThread::join() {
    // This thread may not be an OS-level thread of execution and is thus not
    // joinable. This occurs if the user specifies a greater number of threads
    // than their system can parallelize.
    if (mThread->joinable()) {
        mThread->join();
    }
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
void RenderThread::computePrimaryRay(int x, int y, float xS, float yS, Vec3f &direction, Vec3f &origin) {
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

    mRenderer->mScene.mCamera.computePrimaryRay(pixelX, pixelY, direction, origin);
}


/**
 * std::thread can't accept an instance method so we need this wrapper.
 */
void threadBody(RenderThread *t, Vec3f *image, const int id) {
    t->render(image, id);
}


void RenderThread::run(Vec3f *image, const int id) {
    mThread = std::make_shared<std::thread>(threadBody, this, image, id);
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
 * This is the heart of a RenderThread instance. It will keep popping jobs off
 * the work queue until the queue is empty. I previously attempted to assign
 * interlaced portions of the image to each thread in advance, instead of a
 * having a shared queue. This often led to one thread lagging behind the
 * others and thus wasting potential concurrency.
 */
void RenderThread::render(Vec3f *image, const int id) {
    // Track the total time this thread spent rendering.
    TimePoint startTime = Clock::now();
    mStats.pixels = 0;
    mStats.id = id;

    int start = 0;
    int end = 0;
    while (mRenderer->getWork(start, end)) {
        for (int y = start; y <= end; y++) {
            for (int x = 0; x < mRenderer->mWidth; x++) {
                mStats.pixels++;
                image[y * mRenderer->mWidth + x] = computePixelAverage(x, y);
            }
        }
    }

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


/**
 * Takes care of anti-aliasing and calls `trace` to start the actual ray
 * tracing (finally!).
 */
Vec3f RenderThread::renderPixel(int x, int y) {
    Vec3f direction, origin;
    if (mRenderer->mAntiAliasing == 0) {
        computePrimaryRay(x, y, 0.5f, 0.5f, direction, origin);
        mStats.quantities[PRIMARY]++;
        return trace(origin, direction, 0);
    }

    int s = (int) sqrtf(mRenderer->mAntiAliasing);
    Vec3f color({ 0, 0, 0 });
    for (int ySampling = 0; ySampling < s; ySampling++) {
        for (int xSampling = 0; xSampling < s; xSampling++) {
            float xS = 0;
            float yS = 0;
            computeAntiAliasingSample(s, xSampling, ySampling, xS, yS);
            computePrimaryRay(x, y, xS, yS, direction, origin);

            color = add(color, trace(origin, direction, 0));
            mStats.quantities[PRIMARY]++;
        }
    }

    return divide(color, (float) mRenderer->mAntiAliasing);
}


/**
 * Takes a ray origin and direction and computes the color it accumulates while
 * bouncing around the image. This requires a `depth` argument to ensure that
 * specular and transmission rays are not computed when we've reached
 * `mMaxDepth`.
 *
 * If the ray does not intersect with any object then the background color is
 * returned.
 *
 * Otherwise, the color of a pixel is determined by the material properties of
 * the object this ray intersects. This is the sum of the following components,
 * each scaled by a coefficient defined by the material.
 *
 *   - Ambient, independent of lighting and recursive calls
 *   - Diffuse, dependent on lighting
 *   - Specular, dependent on a recursive `trace` call for reflections
 *   - Transmission, dependent on a recursive `trace` call for refraction
 */
Vec3f RenderThread::trace(Vec3f origin, Vec3f ray, int depth) {
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

    // Ray-object intersection computations only tell us how far along the ray
    // the intersection occurs at, since they just solve the parametric
    // equation of the ray. We must compute the actual point of intersection
    // and the normal of the object at that point.
    Vec3f intersection = add(origin, multiply(ray, intersectionScalar));
    Vec3f normal = intersectionObject->getNormalDir(intersection);
    // The object is responsible for computing its color at a certain point on
    // its surface.
    Vec3f materialColor = intersectionObject->getColor(REST(intersection));
    // The color will always start with its ambient component.
    Vec3f color = multiply(materialColor, intersectionObject->mMaterial->ambient);

    float diffuse = intersectionObject->mMaterial->diffuse;
    if (diffuse > 0) {
        // Every point light in the scene contributes to the color contributed
        // from this ray. A shadow ray is computed by the point light and used
        // to see if the light visible from the object. There may be an object
        // in between the intersection point and every light in the scene, in
        // which case the point is in shadow and no diffuse component
        // contributes to the final color.
        for (auto pointLight : mRenderer->mScene.mPointLights) {
            float distance;
            Vec3f shadowRay = pointLight->direction(
                intersection,
                distance,
                mRenderer->mEnableSoftShadows
            );
            mStats.quantities[SHADOW]++;

            float k;
            // Intersecting with an object that isn't 100% transparent or 100%
            // opaque means that a fraction of this point light intensity
            // contributes to the color. The intensity starts at 1 and each
            // time an object is intersected along the ray's path to the light
            // source, the intensity drops based on the intersection's
            // transparency.
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
                    // We can stop testing objects once the intensity drops
                    // below 0, since it can never increase.
                    if (intensity <= 1e-4) {
                        break;
                    }
                }
            }

            // Use the facing ratio, the shadow intensity computed, the diffuse
            // coefficient of the material, and the facing ratio of the shadow
            // ray and normal to compute the final diffuse contribution from
            // this point light.
            color = add(
                color,
                multiply(
                    materialColor,
                    intensity * pointLight->mIntensity * diffuse * fmax(0, dot(shadowRay, normal))
                )
            );
        }
    }

    bool isTotalInternalReflection = false;
    // Refraction!
    if (intersectionObject->mMaterial->transmission > 0 && depth < mRenderer->mMaxDepth) {
        Vec3f transmissionDirection = computeRefractionDir(
            ray,
            normal,
            intersectionObject->mMaterial->refractiveIndex,
            isTotalInternalReflection
        );
        if (!isTotalInternalReflection) {
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
    // If the refraction computation produced total internal reflection then
    // no recursive call is made and its contribution coefficient should be
    // added to reflection instead.
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


/**
 * Determine whether a ray is coming from inside or outside an object based on
 * its surface normal.
 */
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


void Renderer::printIntro(std::string file) {
    // This is so gross and should be refactored.
    std::cout << "=== Render Info " << file << " ===" << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Target" << "OpenGL, " << mOutputFile << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Image Dimension" << mWidth << " x " << mHeight << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Threads" << mNumThreads << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Max Depth" << mMaxDepth << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Anti-Aliasing" << mAntiAliasing << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Sampling Method";
    if (mAntiAliasing == 0) {
        std::cout << "Off";
    } else if (mAntiAliasingMethod == REGULAR) {
        std::cout << "Regular";
    } else if (mAntiAliasingMethod == RANDOM) {
        std::cout << "Random";
    }
    std::cout << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Soft Shadows?" << (mEnableSoftShadows ? "Yes" : "No") << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Iterations" << mNoiseReduction << std::endl;
    std::cout << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Field of View" << mScene.mCamera.mFieldOfViewRadians * 180.0f / M_PI << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Eye"
              << mScene.mCamera.mPosition[0] << ", "
              << mScene.mCamera.mPosition[1] << ", "
              << mScene.mCamera.mPosition[2]
              << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Focal Point"
              << mScene.mCamera.mLookAt[0] << ", "
              << mScene.mCamera.mLookAt[1] << ", "
              << mScene.mCamera.mLookAt[2]
              << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Aperture Radius";
    if (mScene.mCamera.mApertureRadius == 0) {
        std::cout << "Pinhole";
    } else {
        std::cout << mScene.mCamera.mApertureRadius;
    }
    std::cout << std::endl << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Objects" << mScene.mObjects.size() << std::endl;
    std::cout << std::left << std::setw(20) << std::setfill(' ') << "Lights" << mScene.mPointLights.size() << std::endl;
    std::cout << std::endl << std::endl;
}
