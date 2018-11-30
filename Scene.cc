/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
 * [2] http://www.3dkingdoms.com/weekly/weekly.php?a=2
 * [3] https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
 * [4] https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
 */
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <cmath>
#include <fstream>
#include <memory>

#include "Objects.h"
#include "Scene.h"
#include "Vector.h"


#define MAX_DEPTH 10


Scene::Scene(int width, int height)
: mObjects()
, mPointLights()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    mPointLights.push_back(
        {
            Vec3f({ 0, 2, -1 }),
            { 0, 1, 0, 1 },
            0.7
        }
    );
    mPointLights.push_back(
        {
            Vec3f({ -0.2f, -0.25f, 0.5f }),
            { 0, 1, 0, 1 },
            1
        }
    );
    mPointLights.push_back(
        {
            Vec3f({ 0.2f, -0.25f, 0.5f }),
            { 0, 1, 0, 1 },
            1
        }
    );
    /*mPointLights.push_back(
        {
            Vec3f({ -0.2f, -0.2f, 0 }),
            { 0, 1, 0, 1 },
            0.3
        }
    );*/


    /*
    // Left
    mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 0, 0xb8 / 255.0f, 0x94 / 255.0f }),
            0.3f, 0.6f, 0,
            Vec3f({ -1, 0, 0 }),
            Vec3f({ 1, 0, -0.5f })
        )
    );
    // Right
    mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 1, 0xea / 255.0f, 0xa7 / 255.0f }),
            0.3f, 0.6f, 0,
            Vec3f({ 1, 0, 0 }),
            Vec3f({ -1, 0, -0.5f })
        )
    );
    // Back
    mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 0x55 / 255.0f, 0xef / 255.0f, 0xc4 / 255.0f }),
            0.3f, 0.6f, 0,
            Vec3f({ 0, 0, -3 }),
            Vec3f({ 0, 0, 1 })
        )
    );*/
    // Floor
    mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 0x74 / 255.0f, 0xb9 / 255.0f, 1 }),
            0.4f, 0.6f, 0,
            Vec3f({ 0, -0.5, 0 }),
            Vec3f({ 0, 1, 0 })
        )
    );
    // Ceiling
    /*mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 0xe1 / 255.0f, 0x70 / 255.0f, 0x55 / 255.0f }),
            0.3f, 0.6f, 0.1f,
            Vec3f({ 0, 0.35f, 0 }),
            Vec3f({ 0, -1, -0.5f })
        )
    );*/


    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 0x2d / 255.0f, 0x34 / 255.0f, 0x36 / 255.0f }),
            0, 0, 0.0f, 1.0f,
            Vec3f({ 0, 0, -1.2f }),
            0.4f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 1, 0x76 / 255.0f, 0x75 / 255.0f }),
            0.1f, 0.9f, 0, 0,
            Vec3f({ -0.15f, -0.2f, -0.6f }),
            0.1f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 0x6c / 255.0f, 0x5c / 255.0f, 0xe7 / 255.0f }),
            0.1f, 0.9f, 0, 0,
            Vec3f({ -0.45f, -0.2f, -1.6f }),
            0.2f
        )
    );
    /*
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 1, 0x76 / 255.0f, 0x75 / 255.0f }),
            0, 0, 1,
            Vec3f({ 0.3f, -0.1f, -0.45f }),
            0.05f
        )
    );
    for (int i = 0; i < 50; i++) {
        float x = (float) (rand() % 1000) / 1000.0f - 0.6f;
        float y = (float) (rand() % 1000) / 1000.0f - 0.5f;
        float z = (float) (rand() % 1000) / -1000.0f;
        float r = (float) (rand() % 1000) / 1000.0f;
        float g = (float) (rand() % 1000) / 1000.0f;
        float b = (float) (rand() % 1000) / 1000.0f;
        mObjects.push_back(
            std::make_shared<Sphere>(
                Vec3f({ r, g, b }),
                0.1f, 0.5f, 0.4f,
                Vec3f({ x, y, z }),
                0.01f
            )
        );
    }*/
}


Scene::~Scene() {}


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
Vec3f Scene::computeRay(float aspectRatio, float fovRatio, int x, int y) {
    // Normalize the raster space (mWidth by mHeight pixels) into
    // [0,1] with a 0.5 shift in raster space to center the pixels.
    // NDC assumes (0, 0) is the top-left point.
    float deviceCoordX = (x + 0.5f) / (float) mWidth;
    float deviceCoordY = (y + 0.5f) / (float) mHeight;
    // Centered (0, 0) origin with (1, 1) in the top-right.
    float pixelX = (2 * deviceCoordX - 1) * fovRatio * aspectRatio;
    float pixelY = (1 - 2 * deviceCoordY) * fovRatio;

    return normalize(Vec3f({ pixelX, pixelY, -1 }));
}


/**
 * Renders a scene to a PPM image.
 */
void Scene::render() {
    float aspectRatio = (float) mWidth / (float) mHeight;
    float fovRatio = tan(mCamera.mFieldOfViewRadians / 2.0f);

    std::ofstream img("./Ray.ppm", std::ios::out | std::ios::binary);
    img << "P6\n" << mWidth << " " << mHeight << "\n255\n";
    // Loosely based on [1].
    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            Vec3f ray = computeRay(aspectRatio, fovRatio, x, y);

            Vec3f color = trace(ray);

            img << (unsigned char)(color[0] * 255) <<
                   (unsigned char)(color[1] * 255) <<
                   (unsigned char)(color[2] * 255);
        }
    }
    img.close();
}


Vec3f Scene::trace(Vec3f ray) {
    return trace(Vec3f({0, 0, 0}), ray, 0);
}


bool isInside(Vec3f rayDirection, Vec3f intersectionNormal) {
    return dot(rayDirection, intersectionNormal) > 0;
}


// Based on [2] and [4].
Vec3f computeReflectionDirection(Vec3f incomingRayDirection, Vec3f surfaceNormal) {
    // Projection theorem tells us that any vector is the sum of its
    // projections on to two orthogonal spaces.
    return subtract(
        incomingRayDirection,
        // Project the incoming ray onto the intersection surface normal.
        multiply(surfaceNormal, 2 * dot(incomingRayDirection, surfaceNormal))
    );
}


Vec3f Scene::trace(Vec3f origin, Vec3f ray, int depth) {
    if (fabs(norm(ray) - 1) > 1e-4) {
        printf("norm: %f\n", norm(ray));
    }
    std::shared_ptr<SceneObject> intersectionObject = NULL;
    float intersectionScalar = INFINITY;
    float scalar;

    for (auto obj : mObjects) {
        if (!obj->intersect(origin, ray, scalar)) {
            continue;
        }

        if (scalar < intersectionScalar) {
            intersectionScalar = scalar;
            intersectionObject = obj;
        }
    }

    if (!intersectionObject) {
        return Vec3f({ 0.3f, 0, 0 });
    }

    Vec3f intersection = add(origin, multiply(ray, intersectionScalar));
    Vec3f normal = intersectionObject->getNormalDir(intersection);
    Vec3f color = multiply(
        intersectionObject->getColor(REST(intersection)),
        intersectionObject->mAmbient
    );
    float diffuse = intersectionObject->mDiffuse;

    if (diffuse > 0) {
        float lambertIntensity = 0;
        for (auto pointLight : mPointLights) {
            Vec3f intersectionToLight = subtract(pointLight.mPosition, intersection);
            float d = sqrtf(dot(intersectionToLight, intersectionToLight));
            Vec3f shadowRay = normalize(intersectionToLight);

            float k;
            bool isShadow = false;
            for (auto testObj : mObjects) {
                if (testObj == intersectionObject) {
                    continue;
                }
                // We need to check for just a smidge of bias to ensure we're not
                // intersecting with testObj. We also need to ensure that the
                // distance to the object is less than the distance to the light,
                // since the light could be between the two objects (especially
                // with planes where there is usually an intersection with the
                // ray).
                if (testObj->intersect(intersection, shadowRay, k) && k >= 1e-4 && k < d) {
                    isShadow = true;
                    break;
                }
            }

            if (!isShadow) {
                color = add(
                    color,
                    multiply(
                        intersectionObject->getColor(REST(intersection)),
                        pointLight.mIntensity * diffuse * fmax(0, dot(shadowRay, normal))
                    )
                );
                lambertIntensity += (
                    pointLight.mIntensity *
                    fmax(0, dot(shadowRay, normal))
                );
            }
        }
    }

    bool isTotalInternalReflection = false;
    if (intersectionObject->mTransmission > 0 && depth < MAX_DEPTH) {
        Vec3f transmissionDirection = refractionDir(ray, normal, 1.5f, isTotalInternalReflection);
        if (!isTotalInternalReflection) {
            // The surface normal points away from the sphere. We want the
            // bias to be in the direction of the transmission. If the
            // incoming ray is outside the sphere then the bias should be
            // negating the normal.
            /*float bias = 1e-4;
            if (!isInside(ray, normal)) {
                bias *= -1;
            }*/
            Vec3f transmissionColor = trace(
                add(intersection, multiply(transmissionDirection, 1e-4)),
                transmissionDirection,
                depth + 1
            );
            color = add(
                color,
                multiply(transmissionColor, intersectionObject->mTransmission)
            );
        }
    }

    float intensity = intersectionObject->mSpecular;
    if (isTotalInternalReflection) {
        intensity += intersectionObject->mTransmission;
    }
    if (intensity > 0 && depth < MAX_DEPTH) {
        Vec3f reflectionDirection = computeReflectionDirection(ray, normal);
        Vec3f reflectionColor = trace(
            add(intersection, multiply(reflectionDirection, 1e-5)),
            reflectionDirection,
            depth + 1
        );
        color = add(
            color,
            multiply(reflectionColor, intensity)
        );
    }

    return truncate(color, 1);
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
Vec3f refractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection) {
    float relativeIndexOfRefraction;
    float cosi = -dot(ray, normal);
    if (isInside(ray, normal)) {
        relativeIndexOfRefraction = refractionIndex;
        cosi *= -1;
    } else {
        relativeIndexOfRefraction = 1.0f / refractionIndex;
        normal = multiply(normal, -1);
    }
    assert(cosi > 0);

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


void Scene::setPerspectiveProjectionGL(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        mCamera.mFieldOfViewRadians * (180.0f / M_PI),
        (float) w / (float) h,
        0.1,
        100
    );

    glMatrixMode(GL_MODELVIEW);
}


void Scene::drawObjectsGL() {
    for (auto obj : mObjects) {
        obj->drawGL();
    }
}


void Scene::setLightingParamsGL() {
    for (int i = 0; i < std::min(8ul, mPointLights.size()); i++) {
        glLightfv(
            GL_LIGHT0 + i,
            GL_POSITION,
            mPointLights[i].mPositionGL
        );
    }
}


void Scene::enableLightingGL() {
    for (int i = 0; i < std::min(8ul, mPointLights.size()); i++) {
        glEnable(GL_LIGHT0 + i);
    }
}
