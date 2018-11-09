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
#include <memory>

#include "Objects.h"
#include "Scene.h"
#include "Vector.h"


Scene::Scene(int width, int height)
: mObjects()
, mCamera()
, mWidth(width)
, mHeight(height)
{
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 0, 0}),
            0.05f
        )
    );
}


void Scene::render() {
    float aspectRatio = mWidth / mHeight;
    float fovRatio = tan(mCamera.mFieldOfViewRadians / 2.0f);

    // Loosely based on [1].
    for (int x = 0; x < mWidth; x++) {
        for (int y = 0; y < mHeight; y++) {
            // Normalize the raster space (mWidth by mHeight pixels) into
            // [0,1] with a 0.5 shift in raster space to center the pixels.
            // NDC assumes (0, 0) is the top-left point.
            float deviceCoordX = (x + 0.5f) / (float) mWidth;
            float deviceCoordY = (y + 0.5f) / (float) mHeight;
            // Centered (0, 0) origin with (1, 1) in the top-right.
            float pixelX = (2 * deviceCoordX - 1) * fovRatio * aspectRatio;
            float pixelY = (1 - 2 * deviceCoordY) * fovRatio;

            Vec3f ray = normalize(Vec3f({ pixelX, pixelY, -1 }));

            glVertex3f(0, 0, 0);
            glVertex3f(REST(ray));
        }
    }
}
