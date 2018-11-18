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
#include "../Scene.h"
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


/*
TEST_CASE("Scene computes refraction direction correctly") {
    Scene scene(1, 1);
    Vec3f rayDirection = normalize(Vec3f({ 0, -sinf(M_PI / 6.0f), -1 }));
    printf("Ray direction: %f %f %f\n", REST(rayDirection));
    Vec3f normal({ 0, 1, 0 });
    Vec3f refraction = scene.refractionDir(rayDirection, normal, 1.2f);
    printf("%f %f %f\n", REST(refraction));
    printf("exp: %f %f %f\n", REST(normalize(Vec3f({0, -sinf(M_PI / 6.0f) / 1.2f, -1}))));
    REQUIRE(refraction[1] == -sinf(M_PI / 6.0f) / 1.2f);
}


TEST_CASE("Scene computes refraction direction correctly") {
    Scene scene(1, 1);
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayDirection = normalize(Vec3f({ 0, sinf(M_PI / 6.0f), -1 }));
    float scalar;
    REQUIRE(s.intersect(zero, rayDirection, scalar));
    Vec3f normal = s.getNormalDir(multiply(rayDirection, scalar));
    printf("Normal: %f %f %f\n", REST(normal));

    //Vec3f normal = normalize(Vec3f({ 0, sinf(M_PI / 3.0f), cosf(M_PI / 3.0f) }));
    Vec3f refraction = scene.refractionDir(rayDirection, normal, 1.1);
    printf("%f %f %f\n", REST(refraction));
}
*/
