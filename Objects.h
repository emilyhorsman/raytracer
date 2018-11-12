#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "Vector.h"


class SceneObject {
    public:
        Vec3f mColor;
        GLfloat mColorApproximation[4];
        float mDiffuse;
        float mSpecular;
        Vec3f mOrigin;

        SceneObject(Vec3f color, float diffuse, float specular, Vec3f origin);
        virtual ~SceneObject() = default;

        virtual bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        ) = 0;
        virtual void drawGL() = 0;
};


class Sphere: public SceneObject {
    private:
        float mRadius;

    public:
        Sphere(
            Vec3f color,
            float diffuse,
            float specular,
            Vec3f origin,
            float radius
        );
        bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        );
        void drawGL();
};


#endif
