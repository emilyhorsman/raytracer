/**
 * References:
 *
 * [1] https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
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

#include "Objects.h"


SceneObject::SceneObject(Vec3f color)
: mColor(color)
, mColorApproximation{ color[0], color[1], color[2], 1 }
{}


Sphere::Sphere(Vec3f color, Vec3f origin, float radius)
: SceneObject(color)
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


void Sphere::drawGL() {
    glPushMatrix();
    glTranslatef(REST(mOrigin));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mColorApproximation);
    glutSolidSphere(mRadius, 20, 20);
    glPopMatrix();
}
