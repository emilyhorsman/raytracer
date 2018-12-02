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

#include "Utility.h"
#include "Vector.h"


float getSecondsSince(TimePoint startTime) {
    auto renderAndWriteTime = Clock::now() - startTime;
    return std::chrono::duration_cast<Milli>(renderAndWriteTime).count() / 1000.0f;
}


float randomFloat() {
    return 2.0f * (rand() % 100000) / 100000.0f - 1.0f;
}


float randomUnitFloat() {
    return (rand() % 100000) / 100000.0f;
}


Vec3f randomVec3f() {
    return Vec3f({ randomFloat(), randomFloat(), randomFloat() });
}


bool rayPlaneIntersection(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    Vec3f planePoint,
    Vec3f planeNormal,
    float &intersectionScalar
) {
    float directionDotNormal = dot(planeNormal, rayDirection);
    if (fabs(directionDotNormal) <= 1e-6) {
        return false;
    }

    intersectionScalar = (
        dot(subtract(planePoint, rayOrigin), planeNormal) /
        directionDotNormal
    );

    return intersectionScalar >= 1e-6;
}


Vec3f randomDiskPoint(float z, float r) {
    float R = r * sqrtf(randomUnitFloat());
    float theta = randomUnitFloat() * 2.0f * M_PI;
    return Vec3f({
        R * cosf(theta),
        R * sinf(theta),
        z
    });
}


void renderImageToGL(int width, int height, Vec3f *image) {
    glBegin(GL_POINTS);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            glColor3fv(image[y * width + x].begin());
            glVertex2i(x, height - y);
        }
    }
    glEnd();
}
