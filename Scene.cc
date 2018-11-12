/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
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


Vec3f lightPosition({ 2, 5, 3 });


Scene::Scene(int width, int height)
: mObjects()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({1, 0, 0}),
            Vec3f({0.25f, -0.25f, -0.5}),
            0.25f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 1, 0}),
            Vec3f({-0.5f, 0.5f, -1.5}),
            0.25f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 0, 1}),
            Vec3f({0.0f, 0.0f, -1}),
            0.5f
        )
    );
}


Scene::~Scene() {
}


void Scene::render() {
    float aspectRatio = (float) mWidth / (float) mHeight;
    float fovRatio = tan(mCamera.mFieldOfViewRadians / 2.0f);

    std::ofstream img("./Ray.ppm", std::ios::out | std::ios::binary);
    img << "P6\n" << mWidth << " " << mHeight << "\n255\n";
    // Loosely based on [1].
    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            // Normalize the raster space (mWidth by mHeight pixels) into
            // [0,1] with a 0.5 shift in raster space to center the pixels.
            // NDC assumes (0, 0) is the top-left point.
            float deviceCoordX = (x + 0.5f) / (float) mWidth;
            float deviceCoordY = (y + 0.5f) / (float) mHeight;
            // Centered (0, 0) origin with (1, 1) in the top-right.
            float pixelX = (2 * deviceCoordX - 1) * fovRatio * aspectRatio;
            float pixelY = (1 - 2 * deviceCoordY) * fovRatio;

            Vec3f ray = normalize(Vec3f({ pixelX, pixelY, -1 }));

            Vec3f color = trace(ray);

            img << (unsigned char)(color[0] * 255) <<
                   (unsigned char)(color[1] * 255) <<
                   (unsigned char)(color[2] * 255);
        }
    }
    img.close();
}


Vec3f Scene::trace(Vec3f ray) {
    std::shared_ptr<SceneObject> intersectionObject = NULL;
    float intersectionScalar = INFINITY;
    float scalar;

    for (auto obj : mObjects) {
        if (!obj->intersect(Vec3f({0, 0, 0}), ray, scalar)) {
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
    Vec3f normal = normalize(subtract(intersection, intersectionObject->mOrigin));
    Vec3f shadowRay = subtract(lightPosition, intersection);

    float _k;
    for (auto testObj : mObjects) {
        if (testObj->intersect(intersection, shadowRay, _k)) {
            // Ray-object intersection is in shadow.
            return multiply(intersectionObject->mColor, 0.05f);
        }
    }

    float lambertIntensity = dot(normalize(shadowRay), normal);
    return multiply(
        intersectionObject->mColor,
        fmin(1, fmax(0.05f, lambertIntensity))
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
