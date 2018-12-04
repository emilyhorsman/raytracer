/**
 * @file
 * @brief Mathematical functions on vectors represented with `std::array`
 *        instances.
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <array>


#define REST(v) v[0], v[1], v[2]


typedef std::array<float, 3> Vec3f;


Vec3f crossProduct(Vec3f u, Vec3f v);


Vec3f add(Vec3f u, Vec3f v);


Vec3f add(Vec3f v, float k);


Vec3f subtract(Vec3f u, Vec3f v);


Vec3f subtract(Vec3f v, float k);


float norm(Vec3f v);


Vec3f normalize(Vec3f v);


Vec3f multiply(Vec3f v, float multiple);


Vec3f divide(Vec3f v, float denominator);


float dot(Vec3f u, Vec3f v);


Vec3f truncate(Vec3f v, float maximum);


#endif
