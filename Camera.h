/**
 * @file
 * @brief Maintain camera and lens properties. Instaces can compute a primary
 *        ray from pixel coordinates through a thin lens approximation.
 */
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
        /**
         * Projects a ray in pixel coordinates from an aperture point to the focal
         * plane.
         */
        void computePrimaryRay(float pixelX, float pixelY, Vec3f &direction, Vec3f &origin);
};


#endif
