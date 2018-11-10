#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "Vector.h"


class SceneObject {
    public:
        SceneObject();
        virtual ~SceneObject() = default;

        virtual bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            Vec3f &intersection,
            Vec3f &normal
        ) = 0;
};


class Sphere: public SceneObject {
    private:
        Vec3f mOrigin;
        float mRadius;

    public:
        Sphere(Vec3f origin, float radius);
        bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            Vec3f &intersection,
            Vec3f &normal
        );
};


#endif
