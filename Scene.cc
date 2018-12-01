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
#include <chrono>
#include <cmath>
#include <fstream>
#include <memory>
#include <thread>

#include "Material.h"
#include "Objects.h"
#include "Scene.h"
#include "Stats.h"
#include "Utility.h"
#include "Vector.h"


Scene::Scene()
: mObjects()
, mPointLights()
, mCamera()
{
    mPointLights.push_back(
        std::make_shared<PointLight>(
            Vec3f({ 0.3f, 1.5f, -1.2f }),
            1.8f,
            0.5f
        )
    );

    // Floor
    mObjects.push_back(
        std::make_shared<Plane>(
            std::make_shared<CheckerboardMaterial>(
                Vec3f({ 0x63 / 255.0f, 0x63 / 255.0f, 0x63 / 255.0f }),
                Vec3f({ 0xa2 / 255.0f, 0xab / 255.0f, 0x58 / 255.0f }),
                0.3f, 0.5f, 0.2f, 0, 1, 0.5f
            ),
            Vec3f({ 0, -0.5, 0 }),
            Vec3f({ 0, 1, 0 })
        )
    );

    mObjects.push_back(
        std::make_shared<Sphere>(
            std::make_shared<CheckerboardMaterial>(
                Vec3f({ 1, 1, 1 }),
                Vec3f({ 0, 0, 0 }),
                0.2f, 0.8f, 0, 0, 2, 0.05f
            ),
            Vec3f({ 0.5f, 1, -3.5f }),
            1.5f
        )
    );
    mObjects.push_back(
        std::make_shared<Sphere>(
            std::make_shared<Material>(
                Vec3f({ 0, 0, 1 }),
                0.2f, 0.8f, 0, 0, 2
            ),
            Vec3f({ -0.2f, 0, -1.2f }),
            0.4f
        )
    );

}


Scene::~Scene() {}


/**
 * Renders a scene to a PPM image.
 */
void Scene::render() {
}

bool Scene::getIntersection(
    Vec3f origin,
    Vec3f ray,
    std::shared_ptr<SceneObject> &intersectionObject,
    float &intersectionScalar
) {
    intersectionObject = NULL;
    intersectionScalar = INFINITY;
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

    return intersectionObject != NULL;
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
