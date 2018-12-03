#include <cmath>

#include "PointLight.h"
#include "Utility.h"


PointLight::PointLight(Vec3f position, float intensity, float radius)
: mPosition(position)
, mIntensity(intensity)
, mRadius(radius)
{}


Vec3f PointLight::direction(Vec3f intersection, float &distance, bool useSoftShadows) {
    Vec3f pos = mPosition;
    if (useSoftShadows) {
        // Technique from [10].
        // We can have soft shadows by faking a non-point volume light. This is
        // achieved by randomly jittering the light. This will introduce noise.
        pos = add(pos, multiply(randomVec3f(), mRadius));
    }
    Vec3f i = subtract(pos, intersection);

    distance = sqrtf(dot(i, i));
    return normalize(i);
}
