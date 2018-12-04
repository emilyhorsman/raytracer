/**
 * @file
 * @brief SceneObject and superclass and all subclasses for object types (sphere, plane, etc).
 */
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
        virtual Vec3f getNormalDir(Vec3f intersection) = 0;
        virtual Vec3f getColor(float x, float y, float z);
};


class Sphere : public SceneObject {
    public:
        Vec3f mOrigin;
        float mRadius;

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
        Vec3f getNormalDir(Vec3f intersection);
        Vec3f getColor(float x, float y, float z);
};


class Plane : public SceneObject {
    public:
        Vec3f mPoint;
        Vec3f mNormal;

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
        Vec3f getNormalDir(Vec3f intersection);
};


class Disk : public SceneObject {
    public:
        Vec3f mOrigin;
        Vec3f mNormal;
        float mRadius;

        Disk(
            std::shared_ptr<Material> material,
            Vec3f origin,
            Vec3f normal,
            float radius
        );
        bool intersect(
            Vec3f rayOrigin,
            Vec3f rayDirection,
            float &intersectionScalar
        );
        Vec3f getNormalDir(Vec3f intersection);
};


#endif
