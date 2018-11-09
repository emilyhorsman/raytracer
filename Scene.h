#ifndef _SCENE_H_
#define _SCENE_H_

#include <memory>
#include <vector>


class Scene {
    private:
        std::vector<std::shared_ptr<SceneObject>> mObjects;

    public:
        Scene();
        void render();
};


#endif
