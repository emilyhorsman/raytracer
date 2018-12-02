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
{}


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
