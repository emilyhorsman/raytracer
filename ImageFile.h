#ifndef _IMAGE_FILE_H_
#define _IMAGE_FILE_H_

#include <string>

#include "Vector.h"


void writeImage(const std::string file, const int width, const int height, const Vec3f *image);


#endif
