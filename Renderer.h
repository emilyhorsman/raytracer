#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <thread>

#include "Scene.h"
#include "Stats.h"
#include "Vector.h"


Vec3f computeReflectionDir(Vec3f incomingRayDirection, Vec3f surfaceNormal);
Vec3f computeRefractionDir(Vec3f ray, Vec3f normal, float refractionIndex, bool &isTotalInternalReflection);
bool isInside(Vec3f rayDirection, Vec3f intersectionNormal);


enum AntiAliasingMethod {
    REGULAR,
    RANDOM
};


class Renderer {
    public:
        Scene &mScene;
        int mWidth;
        int mHeight;
        int mNoiseReduction;
        int mMaxDepth;
        int mAntiAliasing;
        AntiAliasingMethod mAntiAliasingMethod;
        bool mEnableSoftShadows;
        int mNumThreads;

        Renderer(
            Scene &scene,
            int width,
            int height,
            int maxDepth,
            int antiAliasing,
            AntiAliasingMethod antiAliasMethod,
            bool enableSoftShadows,
            int noiseReduction,
            int numThreads
        );

        void render();
};


class RenderThread {
    private:
        Vec3f computePrimaryRay(int x, int y, float xS, float yS);
        Vec3f renderPixel(int x, int y);
        Vec3f trace(Vec3f origin, Vec3f ray, int depth);
        Vec3f trace(Vec3f ray);

    public:
        Renderer *mRenderer;
        Stats mStats;
        float mAspectRatio;
        float mFovRatio;
        std::shared_ptr<std::thread> mThread;

        RenderThread(Renderer *renderer, float aspectRatio, float fovRatio);
        void run(Vec3f *image, const int startX, const int step);
        void render(Vec3f *image, const int startX, const int step);
        void join();
};


#endif
