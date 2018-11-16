/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
 * [2] http://www.3dkingdoms.com/weekly/weekly.php?a=2
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
    mPointLights.push_back(
        {
            Vec3f({ 0, 1, 0 }),
            { 0, 1, 0, 1 },
            1
        }
    );

    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({1, 0, 0}),
            0.2f, 0.8f, 0,
            Vec3f({0, 0, -1}),
            0.5f
        )
    );
    mObjects.push_back(
        std::make_shared<Plane>(
            Vec3f({ 1, 1, 1 }),
            0.3, 0.6, 0.1,
            Vec3f({ 0, -0.5, 0 }),
            Vec3f({ 0, 1, 0 })
        )
    );
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
    Vec3f color = Vec3f({ 0, 0, 0 });
    float diffuse = intersectionObject->mDiffuse;

    if (diffuse > 0) {
        float lambertIntensity = 0;
        for (auto pointLight : mPointLights) {
            Vec3f shadowRay = normalize(subtract(pointLight.mPosition, intersection));

            float k;
            bool isShadow = false;
            for (auto testObj : mObjects) {
                // We need to check for just a smidge of bias to ensure we're not
                // intersecting with testObj.
                if (testObj->intersect(intersection, shadowRay, k) && k > 1e-4) {
                    isShadow = true;
                    break;
                }
            }

            if (!isShadow) {
                lambertIntensity += (
                    pointLight.mIntensity *
                    fmax(0, dot(shadowRay, normal))
                );
            }
        }

        color = add(
            color,
            multiply(
                intersectionObject->getColor(REST(intersection)),
                fmax(0, fmin(1, intersectionObject->mAmbient + diffuse * lambertIntensity))
            )
        );
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
        color = add(
            color,
            multiply(reflectionColor, intersectionObject->mSpecular)
        );
    }

    return truncate(color, 1);
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
