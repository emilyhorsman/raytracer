#include "Objects.h"


SceneObject::SceneObject() {}


Sphere::Sphere(Vec3f origin, float radius)
: SceneObject()
, mOrigin(origin)
, mRadius(radius)
{}
