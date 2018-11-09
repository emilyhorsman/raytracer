#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "Vector.h"


class SceneObject {
    public:
        SceneObject();
        virtual ~SceneObject() = default;
};


class Sphere: public SceneObject {
    private:
        Vec3f mOrigin;
        float mRadius;

    public:
        Sphere(Vec3f origin, float radius);
};


#endif
