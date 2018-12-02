#ifndef _SCENE_FILE_H_
#define _SCENE_FILE_H_

#include <string>

#include "Scene.h"


typedef std::map<std::string, std::vector<float>> MaterialProperties;


bool loadSceneFile(Scene &scene, std::string file);


#endif
