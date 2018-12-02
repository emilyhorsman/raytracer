#ifndef _SCENE_FILE_H_
#define _SCENE_FILE_H_

#include <string>

#include "Scene.h"


typedef std::map<std::string, std::vector<float>> MaterialProperties;
typedef std::map<std::string, std::shared_ptr<Material>> Materials;


bool loadSceneFile(Scene &scene, std::string file);


#endif
