/**
 * @file
 * @brief Material and its subclasses are used to store material properties
 *        and for computing the color of a material at a coordinate.
 */
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Vector.h"


class Material {
    public:
        Vec3f color;
        float ambient;
        float diffuse;
        float specular;
        float transmission;
        float refractiveIndex;

        Material(
            Vec3f color,
            float ambient,
            float diffuse,
            float specular,
            float transmission,
            float refractiveIndex
        );
        virtual ~Material() = default;

        virtual Vec3f getColor(float x, float y, float z);
};


class CheckerboardMaterial : public Material {
    public:
        Vec3f oddColor;
        float size;

        CheckerboardMaterial(
            Vec3f evenColor,
            Vec3f oddColor,
            float ambient,
            float diffuse,
            float specular,
            float transmission,
            float refractiveIndex,
            float size
        );

        Vec3f getColor(float x, float y, float z);
};


#endif
