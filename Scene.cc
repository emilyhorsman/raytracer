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

#include "Material.h"
#include "Objects.h"
#include "Scene.h"
#include "Vector.h"


Scene::Scene()
: mObjects()
, mPointLights()
, mCamera()
{
    for (int i = 0; i < 50; i++) {
        float x = (float) (rand() % 1000) / 1000.0f - 0.6f;
        float y = (float) (rand() % 1000) / 1000.0f - 0.5f;
        float z = (float) (rand() % 1000) / -1000.0f;
        float r = (float) (rand() % 1000) / 1000.0f;
        float g = (float) (rand() % 1000) / 1000.0f;
        float b = (float) (rand() % 1000) / 1000.0f;
        mObjects.push_back(
            std::make_shared<Sphere>(
                std::make_shared<Material>(
                    Vec3f({ r, g, b }),
                    0.1f, 0.5f, 0.4f, 0, 1
                ),
                Vec3f({ x, y, z }),
                0.01f
            )
        );
    }
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
