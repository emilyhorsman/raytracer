#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "Vector.h"


class PointLight {
    public:
        Vec3f mPosition;
        float mIntensity;
        float mRadius;

        PointLight(Vec3f position, float intensity, float radius);
        Vec3f direction(Vec3f intersection, float &distance, bool useSoftShadows);
};


#endif
