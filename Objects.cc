#include "Objects.h"


SceneObject::SceneObject() {}


Sphere::Sphere(Vec3f origin, float radius)
: SceneObject()
, mOrigin(origin)
, mRadius(radius)
{}


bool Sphere::intersect(Vec3f rayOrigin, Vec3f rayDirection) {
    Vec3f raySphereSegment = subtract(mOrigin, rayOrigin);
    // This is the length of the projection of the ray-sphere line segment
    // onto the ray.
    float raySphereProjectionNorm = dot(raySphereSegment, rayDirection);

    float discriminant = (
        mRadius * mRadius -
        dot(raySphereSegment, raySphereSegment) +
        raySphereProjectionNorm * raySphereProjectionNorm
    );
    return discriminant >= 0;
}
