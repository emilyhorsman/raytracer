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


SceneObject::SceneObject(Vec3f color, float ambient, float diffuse, float specular)
: mColor(color)
, mColorApproximation{ color[0], color[1], color[2], 1 }
, mAmbient(ambient)
, mDiffuse(diffuse)
, mSpecular(specular)
{}


Sphere::Sphere(
    Vec3f color,
    float ambient,
    float diffuse,
    float specular,
    Vec3f origin,
    float radius
)
: SceneObject(color, ambient, diffuse, specular)
, mOrigin(origin)
, mRadius(radius)
{}


// Loosely based on [1].
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


void Sphere::drawGL() {
    glPushMatrix();
    glTranslatef(REST(mOrigin));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mColorApproximation);
    glutSolidSphere(mRadius, 20, 20);
    glPopMatrix();
}


Vec3f Sphere::getNormalDir(Vec3f intersection) {
    return normalize(subtract(intersection, mOrigin));
}


Vec3f Sphere::getColor(float _x, float _y, float _z) {
    return mColor;
}


Plane::Plane(
    Vec3f color,
    float ambient,
    float diffuse,
    float specular,
    Vec3f point,
    Vec3f normal
)
: SceneObject(color, ambient, diffuse, specular)
, mPoint(point)
, mNormal(normal)
{}


bool Plane::intersect(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    float &intersectionScalar
) {
    float directionDotNormal = dot(mNormal, rayDirection);
    if (fabs(directionDotNormal) <= 1e-6) {
        return false;
    }

    intersectionScalar = (
        dot(subtract(mPoint, rayOrigin), mNormal) /
        directionDotNormal
    );

    return intersectionScalar >= 0;
}


float Plane::computeY(float x, float z) {
    float constantTerm = -dot(mPoint, mNormal);
    float y = (
        (-mNormal[0] * x - mNormal[2] * z - constantTerm) /
        mNormal[1]
    );
    return y;
}


void Plane::drawGL() {
    glBegin(GL_QUADS);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mColorApproximation);
    glNormal3fv(mNormal.begin());
    // TODO: We have to write one of each of these depending on the normal.
    glVertex3f(-5, computeY(-5, 5), 5);
    glVertex3f(5, computeY(5, 5), 5);
    glVertex3f(5, computeY(5, -5), -5);
    glVertex3f(-5, computeY(-5, -5), -5);
    glEnd();
}


Vec3f Plane::getNormalDir(Vec3f intersection) {
    return mNormal;
}


Vec3f Plane::getColor(float x, float y, float z) {
    int checkerX = (int) (x * 10);
    int checkerZ = (int) (z * 10);

    if ((checkerX + checkerZ) % 2 == 0) {
        return Vec3f({ 0.7, 0.7, 0.7 });
    } else {
        return Vec3f({ 1, 1, 1 });
    }
}
