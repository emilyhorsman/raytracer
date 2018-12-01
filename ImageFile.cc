#include <fstream>
#include <string>

#include "ImageFile.h"
#include "Vector.h"


void writeImage(const std::string file, const int width, const int height, const Vec3f *image) {
    std::ofstream img(file, std::ios::out | std::ios::binary);
    img << "P6\n" << width << " " << height << "\n255\n";
    for (int i = 0; i < width * height; i++) {
        img << (unsigned char)(image[i][0] * 255) <<
               (unsigned char)(image[i][1] * 255) <<
               (unsigned char)(image[i][2] * 255);
    }
    img.close();
}
