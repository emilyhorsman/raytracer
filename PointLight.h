/**
 * @file
 * @brief PointLight instances represent the position, intensity, and radius of
 *        a light. Computes direction from intersection with soft shadow logic.
 */
#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include "Vector.h"


class PointLight {
    public:
        Vec3f mPosition;
        /**
         * @brief The scalar for any color contribution from this light.
         */
        float mIntensity;
        /**
         * @brief The radius of this light's spherical volume. This value does
         *        not matter if soft shadows are turned off.
         */
        float mRadius;

        PointLight(Vec3f position, float intensity, float radius);
        Vec3f direction(Vec3f intersection, float &distance, bool useSoftShadows);
};


#endif
