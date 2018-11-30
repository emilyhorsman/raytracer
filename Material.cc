#include <math.h>

#include "Material.h"


Material::Material(
    Vec3f color,
    float ambient,
    float diffuse,
    float specular,
    float transmission,
    float refractiveIndex
):
color(color),
ambient(ambient),
diffuse(diffuse),
specular(specular),
transmission(transmission),
refractiveIndex(refractiveIndex)
{}


Vec3f Material::getColor(float x, float y, float z) {
    return color;
}


CheckerboardMaterial::CheckerboardMaterial(
    Vec3f evenColor,
    Vec3f oddColor,
    float ambient,
    float diffuse,
    float specular,
    float transmission,
    float refractiveIndex,
    float size
)
: Material(
    evenColor,
    ambient,
    diffuse,
    specular,
    transmission,
    refractiveIndex
)
, oddColor(oddColor)
, size(size)
{}


Vec3f CheckerboardMaterial::getColor(float x, float y, float z) {
    int k = (
        (int) floor(x / size) +
        (int) floor(y / size) +
        (int) floor(z / size)
    );
    if (k % 2 == 0) {
        return color;
    } else {
        return oddColor;
    }
}
