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
