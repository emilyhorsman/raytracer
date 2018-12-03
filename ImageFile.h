/**
 * @file
 * @brief Writes an array of color vectors to a PPM image.
 */
#ifndef _IMAGE_FILE_H_
#define _IMAGE_FILE_H_

#include <string>

#include "Vector.h"


void writeImage(const std::string file, const int width, const int height, const Vec3f *image);


#endif
