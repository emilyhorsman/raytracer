#include <memory>

#include "Objects.h"
#include "Scene.h"
#include "Vector.h"


Scene::Scene()
: mObjects()
{
    mObjects.push_back(
        std::make_shared<Sphere>(
            Vec3f({0, 0, 0}),
            0.05f
        )
    );
}
