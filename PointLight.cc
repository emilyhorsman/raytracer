#include <cmath>

#include "PointLight.h"
#include "Utility.h"


PointLight::PointLight(Vec3f position, float intensity, float radius)
: mPosition(position)
, mIntensity(intensity)
, mRadius(radius)
{}


/**
 * Compute the direction vector between the point `intersection` and this light.
 * Calling this function will populate the `distance` reference which can be
 * used to check if a light is between two objects. For instance, if a primary
 * ray hits object A and its shadow ray hits object B then the point is only in
 * shadow if the distance to object B is less than the distance to the
 * PointLight.
 *
 * @param useSoftShadows Jitter the light within the radius if enabled.
 */
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
