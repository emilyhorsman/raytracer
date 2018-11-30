#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "Material.h"
#include "Vector.h"


class SceneObject {
    public:
        std::shared_ptr<Material> mMaterial;

        SceneObject(std::shared_ptr<Material> material);
        virtual ~SceneObject() = default;

        virtual bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        ) = 0;
        virtual void drawGL() = 0;
        virtual Vec3f getNormalDir(Vec3f intersection) = 0;
        virtual Vec3f getColor(float x, float y, float z);
};


class Sphere : public SceneObject {
    private:
        Vec3f mOrigin;
        float mRadius;

    public:
        Sphere(
            std::shared_ptr<Material> material,
            Vec3f origin,
            float radius
        );
        bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        );
        void drawGL();
        Vec3f getNormalDir(Vec3f intersection);
        Vec3f getColor(float x, float y, float z);
};


class Plane : public SceneObject {
    private:
        Vec3f mPoint;
        Vec3f mNormal;
        float computeY(float x, float z);

    public:
        Plane(
            std::shared_ptr<Material> material,
            Vec3f point,
            Vec3f normal
        );
        bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        );
        void drawGL();
        Vec3f getNormalDir(Vec3f intersection);
};


#endif
