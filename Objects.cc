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

#include "Objects.h"
#include "Utility.h"


SceneObject::SceneObject(std::shared_ptr<Material> material)
: mMaterial(material)
{}


Vec3f SceneObject::getColor(float x, float y, float z) {
    return mMaterial->getColor(x, y, z);
}


Sphere::Sphere(std::shared_ptr<Material> material, Vec3f origin, float radius)
: SceneObject(material)
, mOrigin(origin)
, mRadius(radius)
{}


// Loosely based on [8].
bool Sphere::intersect(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    float &intersectionScalar
) {
    Vec3f raySphereSegment = subtract(mOrigin, rayOrigin);
    // This is the length of the projection of the ray-sphere line segment
    // onto the ray.
    float raySphereProjectionNorm = dot(raySphereSegment, rayDirection);
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
        intersectionScalar = scalarA;
    } else if (scalarB > 0) {
        intersectionScalar = scalarB;
    } else {
        return false;
    }

    return true;
}


Vec3f Sphere::getNormalDir(Vec3f intersection) {
    return normalize(subtract(intersection, mOrigin));
}


/**
 * Perform some texture mapping so that we can use the CheckerboardMaterial
 * without it being distorted.
 */
Vec3f Sphere::getColor(float x, float y, float z) {
    // Based on [9].
    float theta = atan2(-(z - mOrigin[2]), x - mOrigin[0]);
    float u = (theta + M_PI) / (2.0f * M_PI);
    float phi = acos(-(y - mOrigin[1]) / mRadius);
    float v = phi / M_PI;
    return mMaterial->getColor(u, v, 0);
}


Plane::Plane(std::shared_ptr<Material> material, Vec3f point, Vec3f normal)
: SceneObject(material)
, mPoint(point)
, mNormal(normalize(normal))
{}


bool Plane::intersect(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    float &intersectionScalar
) {
    return rayPlaneIntersection(rayOrigin, rayDirection, mPoint, mNormal, intersectionScalar);
}


Vec3f Plane::getNormalDir(Vec3f intersection) {
    return mNormal;
}


Disk::Disk(std::shared_ptr<Material> material, Vec3f origin, Vec3f normal, float radius)
: SceneObject(material)
, mOrigin(origin)
, mNormal(normalize(normal))
, mRadius(radius)
{}


Vec3f Disk::getNormalDir(Vec3f intersection) {
    return mNormal;
}


bool Disk::intersect(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    float &intersectionScalar
) {
    if (!rayPlaneIntersection(rayOrigin, rayDirection, mOrigin, mNormal, intersectionScalar)) {
        return false;
    }

    Vec3f intersection = add(rayOrigin, multiply(rayDirection, intersectionScalar));
    Vec3f difference = subtract(intersection, mOrigin);
    return sqrtf(dot(difference, difference)) < mRadius;
}
