#ifndef _SCENE_FILE_H_
#define _SCENE_FILE_H_

#include <map>
#include <string>

#include "Renderer.h"
#include "Scene.h"


typedef std::map<std::string, std::vector<float>> FloatProperties;
typedef std::map<std::string, std::shared_ptr<Material>> Materials;


bool loadSceneFile(Renderer &renderer, Scene &scene, std::string file);


#endif
