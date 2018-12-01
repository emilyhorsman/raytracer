#ifndef _SCENE_H_
#define _SCENE_H_

#include <memory>
#include <vector>

#include "Camera.h"
#include "Objects.h"
#include "PointLight.h"
#include "Stats.h"

Vec3f refractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection);


enum AntiAliasingMethod {
    REGULAR,
    RANDOM
};


class Scene {
    private:
        std::vector<std::shared_ptr<SceneObject>> mObjects;
        std::vector<std::shared_ptr<PointLight>> mPointLights;
        Camera mCamera;
        int mMaxDepth;
        int mAntiAliasing;
        AntiAliasingMethod mAntiAliasingMethod;
        bool mEnableSoftShadows;

        Vec3f computeRay(float aspectRatio, float fovRatio, int x, int y, float xS, float yS);
        Vec3f trace(Stats &stats, Vec3f origin, Vec3f ray, int depth);
        Vec3f trace(Stats &stats, Vec3f ray);

    public:
        int mWidth;
        int mHeight;
        int mNoiseReduction;

        Scene(
            int width,
            int height,
            int maxDepth,
            int antiAliasing,
            AntiAliasingMethod antiAliasMethod,
            bool enableSoftShadows,
            int noiseReduction
        );
        ~Scene();
        void render();
        void setPerspectiveProjectionGL(int w, int h);
        void drawObjectsGL();
        void setLightingParamsGL();
        void enableLightingGL();
        Vec3f renderPixel(Stats &stats, float aspectRatio, float fovRatio, int x, int y);
};


#endif
