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


Vec3f lightPosition({ 0, 0, 0 });


Scene::Scene(int width, int height)
: mObjects()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({1, 0, 0}),
            Vec3f({0, 0, -1.5f}),
            0.3f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 0, 1}),
            Vec3f({0.5f, 0.5f, -1.5f}),
            0.25f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 1, 0}),
            Vec3f({-0.5f, 0.5f, -1}),
            0.25f
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
            glColor3f(REST(color));
            glVertex3f(x, mHeight - y, 0);

            img << (unsigned char)(color[0] * 255) <<
                   (unsigned char)(color[1] * 255) <<
                   (unsigned char)(color[2] * 255);
        }
    }
    img.close();
}


Vec3f Scene::trace(Vec3f ray) {
    for (auto obj : mObjects) {
        Vec3f intersection;
        Vec3f normal;
        if (!obj->intersect(Vec3f({0, 0, 0}), ray, intersection, normal)) {
            continue;
        }

        Vec3f shadowRay = subtract(lightPosition, intersection);
        Vec3f _i, _n;
        for (auto testObj : mObjects) {
            if (testObj->intersect(intersection, shadowRay, _i, _n)) {
                // Ray-object intersection is in shadow.
                return multiply(obj->mColor, 0.05f);
            }
        }

        float lambertIntensity = dot(normalize(shadowRay), normal);
        return multiply(obj->mColor, fmin(1, 0.05 + lambertIntensity));
    }

    // The ray didn't intersect with any object.
    return Vec3f({ 0, 0, 0 });
}
