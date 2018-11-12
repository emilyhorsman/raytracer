/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
 */
#include <cmath>

#include "Objects.h"


SceneObject::SceneObject() {}


Sphere::Sphere(Vec3f origin, float radius)
: SceneObject()
, mOrigin(origin)
, mRadius(radius)
{}


// Loosely based on [1].
bool Sphere::intersect(Vec3f rayOrigin, Vec3f rayDirection, Vec3f &intersection, Vec3f &normal) {
    Vec3f raySphereSegment = subtract(mOrigin, rayOrigin);
    // This is the length of the projection of the ray-sphere line segment
    // onto the ray.
    float raySphereProjectionNorm = dot(raySphereSegment, rayDirection);
    if (raySphereProjectionNorm < 0) {
        return false;
    }

    float discriminant = (
        dot(raySphereSegment, raySphereSegment) -
        raySphereProjectionNorm * raySphereProjectionNorm
    );
    float radiusSq = mRadius * mRadius;
    if (discriminant > radiusSq) {
        return false;
    }

    float circleDelta = sqrt(radiusSq - discriminant);
    float scalarA = raySphereProjectionNorm - circleDelta;
    float scalarB = raySphereProjectionNorm + circleDelta;
    if (scalarA < scalarB && scalarA > 0) {
        intersection = add(rayOrigin, multiply(rayDirection, scalarA));
    } else if (scalarB > 0) {
        intersection = add(rayOrigin, multiply(rayDirection, scalarB));
    } else {
        return false;
    }

    normal = normalize(subtract(intersection, mOrigin));

    return true;
}
