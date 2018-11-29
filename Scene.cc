/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
 * [2] http://www.3dkingdoms.com/weekly/weekly.php?a=2
 * [3] https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
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


#define MAX_DEPTH 3


Scene::Scene(int width, int height)
: mObjects()
, mPointLights()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    /*mPointLights.push_back(
        {
            Vec3f({ 0, 0.5f, -1 }),
            { 0, 1, 0, 1 },
            0.7
        }
    );*/
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
            0.2f, 0.8f, 0,
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
            0, 0, 0.1f, 0.9f,
            Vec3f({ 0, -0.25f, -1 }),
            0.25f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 1, 0x76 / 255.0f, 0x75 / 255.0f }),
            0.1f, 0.9f, 0, 0,
            Vec3f({ -0.2f, -0.2f, -1.5f }),
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
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({ 0x6c / 255.0f, 0x5c / 255.0f, 0xe7 / 255.0f }),
            0.1f, 0.8f, 0.1f,
            Vec3f({ 0.1f, -0.2f, -0.4f }),
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


Vec3f Scene::trace(Vec3f origin, Vec3f ray, int depth) {
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
        return Vec3f({ 0, 0, 0 });
    }

    Vec3f intersection = multiply(ray, intersectionScalar);
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
        Vec3f transmissionDirection = refractionDir(ray, normal, 1.2f, isTotalInternalReflection);
        if (!isTotalInternalReflection) {
            float bias = 1 + 1e-4;
            if (isInside(ray, normal)) {
                bias *= -1;
            }
            Vec3f transmissionColor = trace(
                add(intersection, multiply(normal, bias)),
                transmissionDirection,
                depth + 1
            );
            color = add(
                color,
                multiply(transmissionColor, intersectionObject->mTransmission)
            );
        }
    }


    if (intersectionObject->mSpecular > 0 && depth < MAX_DEPTH) {
        // Reflection direction computation based on [2].
        Vec3f reflectionDiretion = subtract(
            ray,
            multiply(
                normal,
                2 * dot(ray, normal)
            )
        );
        Vec3f reflectionColor = trace(
            add(intersection, multiply(normal, 1e-4)),
            reflectionDiretion,
            depth + 1
        );
        float intensity = intersectionObject->mSpecular;
        if (isTotalInternalReflection) {
            intensity += intersectionObject->mTransmission;
        }
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
Vec3f Scene::refractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection) {
    // (1) Setup the reflection normal and ratio of refraction indices from
    //     Snell's law.
    float projectionOntoNormalLength = dot(ray, normal);
    bool isIncidentInsideIntersection = projectionOntoNormalLength > 0;
    Vec3f n = normal;
    // in / out
    float relativeIndexOfRefraction;
    if (isIncidentInsideIntersection) {
        // We should be reflecting across a normal inside the object, so
        // re-orient the normal to be inside.
        n = multiply(n, -1);
        projectionOntoNormalLength *= -1;
        // The ray is transiting from the object medium to air, thus
        // refractionIndex / 1.
        relativeIndexOfRefraction = refractionIndex;
    } else {
        // The ray is transiting from air to the object medium.
        relativeIndexOfRefraction = 1.0f / refractionIndex;
    }


    // (2) If the reflection dictated by Snell's law causes the transmission
    //     ray to stay in the origin medium then there is no refraction. This
    //     happens when the incident ray is at a "critical angle" with the
    //     normal.
    float base = (
        1 - relativeIndexOfRefraction * relativeIndexOfRefraction *
        (1 - projectionOntoNormalLength * projectionOntoNormalLength)
    );
    if (base < 0) {
        return ray;
    }

    // (3) Now we can actually compute the direction of the transmission ray.
    return normalize(add(
        multiply(
            ray,
            relativeIndexOfRefraction
        ),
        multiply(
            n,
            relativeIndexOfRefraction * projectionOntoNormalLength - sqrtf(base)
        )
    ));
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
