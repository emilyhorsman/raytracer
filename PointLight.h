#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "Vector.h"


struct PointLight {
    Vec3f mPosition;
    GLfloat mPositionGL[4];
    float mIntensity;
};


typedef struct PointLight PointLight;


#endif
