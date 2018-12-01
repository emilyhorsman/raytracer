#ifndef _SCENE_H_
#define _SCENE_H_

#include <memory>
#include <vector>

#include "Camera.h"
#include "Objects.h"
#include "PointLight.h"

Vec3f refractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection);


enum AntiAliasingMethod {
    REGULAR,
    RANDOM
};


class Scene {
    private:
        std::vector<std::shared_ptr<SceneObject>> mObjects;
        std::vector<PointLight> mPointLights;
        Camera mCamera;
        int mWidth;
        int mHeight;
        int mMaxDepth;
        int mAntiAliasing;
        AntiAliasingMethod mAntiAliasingMethod;
        int mNumPrimaryRays;
        int mNumIncidentRays;
        int mNumSpecularRays;
        int mNumTransmissionRays;
        int mNumIntersections;

        Vec3f computeRay(float aspectRatio, float fovRatio, int x, int y, float xS, float yS);
        Vec3f trace(Vec3f origin, Vec3f ray, int depth);
        Vec3f trace(Vec3f ray);
        Vec3f renderPixel(float aspectRatio, float fovRatio, int x, int y);

    public:
        Scene(int width, int height, int maxDepth, int antiAliasing, AntiAliasingMethod antiAliasMethod);
        ~Scene();
        void render();
        void setPerspectiveProjectionGL(int w, int h);
        void drawObjectsGL();
        void setLightingParamsGL();
        void enableLightingGL();
};


#endif
