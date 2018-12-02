#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <chrono>

#include "Vector.h"


typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;
typedef std::chrono::milliseconds Milli;


float getSecondsSince(TimePoint startTime);


float randomFloat();
Vec3f randomVec3f();
Vec3f randomDiskPoint(float z, float r);


bool rayPlaneIntersection(
    Vec3f rayOrigin,
    Vec3f rayDirection,
    Vec3f planePoint,
    Vec3f planeNormal,
    float &intersectionScalar
);


#endif
