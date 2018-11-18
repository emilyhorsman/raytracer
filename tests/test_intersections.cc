#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include "../Objects.h"
#include "../Vector.h"


Vec3f zero({ 0, 0, 0 });


TEST_CASE("Ray-sphere intersection from outside") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin = zero;
    Vec3f rayDirection({ 0, 0, -1 });
    float scalar;
    REQUIRE(s.intersect(rayOrigin, rayDirection, scalar));
    REQUIRE(scalar == 0.5f);
    Vec3f normal = s.getNormalDir(multiply(rayDirection, scalar));
    REQUIRE(normal[0] == 0);
    REQUIRE(normal[1] == 0);
    REQUIRE(normal[2] == 1);
}


TEST_CASE("Ray-sphere intersection from outside with more complicated angle") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin = zero;
    Vec3f rayDirection({ 0.1f, 0.2f, -1 });
    float scalar;
    REQUIRE(s.intersect(rayOrigin, rayDirection, scalar));
}



TEST_CASE("Ray-sphere intersection from inside") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin({ 0, 0, -1.25f });
    Vec3f rayDirection({ 0, 0, -1 });
    float scalar;
    REQUIRE(s.intersect(rayOrigin, rayDirection, scalar));
    REQUIRE(scalar == 0.25f);
    Vec3f normal = s.getNormalDir(multiply(rayDirection, scalar));
    REQUIRE(normal[0] == 0);
    REQUIRE(normal[1] == 0);
    REQUIRE(normal[2] == 1);
}


TEST_CASE("Ray-sphere intersection fails with ray origin in front of object") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin({ 0, 0, -1.5f - 1e-6 });
    Vec3f rayDirection({ 0, 0, -1 });
    float scalar;
    REQUIRE(!s.intersect(rayOrigin, rayDirection, scalar));
}


TEST_CASE("Ray-sphere intersection fails with ray direction missing object") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin = zero;
    Vec3f rayDirection = normalize(Vec3f({ 0, 0.6f, -1 }));
    float scalar;
    REQUIRE(!s.intersect(rayOrigin, rayDirection, scalar));
}
