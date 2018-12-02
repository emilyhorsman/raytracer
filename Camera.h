#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vector.h"


class Camera {
    public:
        float mFieldOfViewRadians;
        Vec3f mPosition;
        Vec3f mLookAt;
        float mApertureRadius;

        Camera();
};


#endif
