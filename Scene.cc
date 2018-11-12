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


Vec3f lightPosition({ 0, 0, 0 });


Scene::Scene(int width, int height)
: mObjects()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 0, 1}),
            0.4, 0.6,
            Vec3f({0.3f, 0, -1}),
            0.35f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({1, 0, 0}),
            0.8, 0.2,
            Vec3f({-0.5f, 0.5f, -1.5}),
            0.3f
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
    Vec3f normal = normalize(subtract(intersection, intersectionObject->mOrigin));
    Vec3f shadowRay = subtract(lightPosition, intersection);
    Vec3f color = Vec3f({ 0, 0, 0 });

    float diffuse = intersectionObject->mDiffuse;
    float _k;
    for (auto testObj : mObjects) {
        if (testObj->intersect(intersection, shadowRay, _k)) {
            // Ray-object intersection is in shadow.
            diffuse = 0;
            break;
        }
    }

    if (diffuse > 0) {
        float lambertIntensity = dot(normalize(shadowRay), normal);
        color = add(
            color,
                multiply(
                intersectionObject->mColor,
                intersectionObject->mDiffuse * fmax(0, lambertIntensity)
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
        Vec3f reflectionColor = trace(intersection, reflectionDiretion, depth + 1);
        color = add(
            color,
            multiply(reflectionColor, intersectionObject->mSpecular)
        );
    }

    return color;
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
