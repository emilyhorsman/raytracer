#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <memory>
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


/**
 * Responsibilities:
 *
 *   - Rendering settings (anti-aliasing, image dimensions, etc)
 *   - Producing and writing an image
 *   - Creating RenderThread instances
 */
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


/**
 * Responsibilities:
 *
 *   - Rendering a portion of the image in a std::thread
 *   - Keeping statistics on a rendering
 *   - Ray tracing engine
 *
 * It'd be nice to further decouple this.
 */
class RenderThread {
    private:
        Vec3f computePrimaryRay(int x, int y, float xS, float yS);
        Vec3f renderPixel(int x, int y);
        Vec3f trace(Vec3f origin, Vec3f ray, int depth);
        Vec3f trace(Vec3f ray);
        Vec3f computePixelAverage(int x, int y);
        void computeAntiAliasingSample(int samples, int x, int y, float &xS, float &yS);

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
