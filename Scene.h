#ifndef _SCENE_H_
#define _SCENE_H_

#include <memory>
#include <vector>

#include "Camera.h"
#include "Objects.h"


class Scene {
    private:
        std::vector<std::shared_ptr<SceneObject>> mObjects;
        Camera mCamera;
        int mWidth;
        int mHeight;

    public:
        Scene(int width, int height);
        void render();
};


#endif
