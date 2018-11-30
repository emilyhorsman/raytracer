#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Vector.h"


struct Material {
    Vec3f color;
    float ambient;
    float diffuse;
    float specular;
    float transmission;
    float refractiveIndex;
};


typedef struct Material Material;


#endif
