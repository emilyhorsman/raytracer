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
    Sphere s(zero, 0, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
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
    Sphere s(zero, 0, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin = zero;
    Vec3f rayDirection({ 0.1f, 0.2f, -1 });
    float scalar;
    REQUIRE(s.intersect(rayOrigin, rayDirection, scalar));
}



TEST_CASE("Ray-sphere intersection from inside") {
    Sphere s(zero, 0, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
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
    Sphere s(zero, 0, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin({ 0, 0, -1.5f - 1e-6 });
    Vec3f rayDirection({ 0, 0, -1 });
    float scalar;
    REQUIRE(!s.intersect(rayOrigin, rayDirection, scalar));
}


TEST_CASE("Ray-sphere intersection fails with ray direction missing object") {
    Sphere s(zero, 0, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin = zero;
    Vec3f rayDirection = normalize(Vec3f({ 0, 0.6f, -1 }));
    float scalar;
    REQUIRE(!s.intersect(rayOrigin, rayDirection, scalar));
}


TEST_CASE("Ray-plane intersection 1") {
    Plane p(zero, 0, 0, 0, Vec3f({ 0, 0, -2 }), Vec3f({ 0, 0, 1 }));
    float scalar;
    REQUIRE(p.intersect(zero, Vec3f({ 0, 0, -1 }), scalar));
    REQUIRE(scalar == 2);
    REQUIRE(p.intersect(zero, Vec3f({ 1, 1, -1 }), scalar));
    REQUIRE(p.intersect(zero, Vec3f({ 1, -1, -1 }), scalar));
}


TEST_CASE("Ray-plane intersection misses 1") {
    Plane p(zero, 0, 0, 0, Vec3f({ 0, 0, -2 }), Vec3f({ 0, 0, 1 }));
    float scalar;
    REQUIRE(!p.intersect(zero, Vec3f({ 0, 0, 1 }), scalar));
}


TEST_CASE("Refraction straight through center from outside") {
    Vec3f rayDirection({ 0, 0, -1 });
    Vec3f normal({ 0, 0, 1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.2f, isTotalInternalReflection);
    REQUIRE(refraction[0] == 0);
    REQUIRE(refraction[1] == 0);
    REQUIRE(refraction[2] == -1);
    REQUIRE(!isTotalInternalReflection);
}


TEST_CASE("Refraction straight through center from inside") {
    Vec3f rayDirection({ 0, 0, -1 });
    Vec3f normal({ 0, 0, -1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.2f, isTotalInternalReflection);
    REQUIRE(refraction[0] == 0);
    REQUIRE(refraction[1] == 0);
    REQUIRE(refraction[2] == -1);
    REQUIRE(!isTotalInternalReflection);
}


TEST_CASE("Refraction at 45 degrees through glass slab") {
    Vec3f rayDirection = normalize(Vec3f({ 0, -sinf(M_PI / 4.0f), -cosf(M_PI / 4.0f) }));
    Vec3f normal({ 0, 0, 1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    REQUIRE(refraction[0] == 0);
    REQUIRE(refraction[1] == Approx(-sinf(M_PI / 6.0f)).margin(0.03f));
    REQUIRE(refraction[2] == Approx(-cosf(M_PI / 6.0f)).margin(0.03f));
    REQUIRE(!isTotalInternalReflection);

    refraction = refractionDir(refraction, multiply(normal, -1), 1.5f, isTotalInternalReflection);
    REQUIRE(refraction[0] == Approx(rayDirection[0]));
    REQUIRE(refraction[1] == Approx(rayDirection[1]));
    REQUIRE(refraction[2] == Approx(rayDirection[2]));
    REQUIRE(!isTotalInternalReflection);
}


TEST_CASE("Refraction at 75 degrees from normal through glass slab") {
    Vec3f rayDirection = normalize(Vec3f({ 0, -sinf(5.0f * M_PI / 12.0f), -cosf(5.0f * M_PI / 12.0f) }));
    Vec3f normal({ 0, 0, 1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    REQUIRE(refraction[0] == 0);
    REQUIRE(refraction[1] == Approx(-sinf(40.0f * M_PI / 180.0f)).margin(0.03f));
    REQUIRE(refraction[2] == Approx(-cosf(40.0f * M_PI / 180.0f)).margin(0.03f));
    REQUIRE(!isTotalInternalReflection);

    refraction = refractionDir(refraction, multiply(normal, -1), 1.5f, isTotalInternalReflection);
    REQUIRE(refraction[0] == Approx(rayDirection[0]));
    REQUIRE(refraction[1] == Approx(rayDirection[1]));
    REQUIRE(refraction[2] == Approx(rayDirection[2]));
    REQUIRE(!isTotalInternalReflection);
}


TEST_CASE("Refraction at 45 degrees from normal below glass slab") {
    Vec3f rayDirection = normalize(Vec3f({ 0, sinf(M_PI / 4.0f), -cosf(M_PI / 4.0f) }));
    printf("ray direction: %f %f %f\n", REST(rayDirection));
    Vec3f normal({ 0, 0, 1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    printf("RESULT: %f %f %f %d %f\n", REST(refraction), isTotalInternalReflection, norm(refraction));
    REQUIRE(refraction[0] == 0);
    REQUIRE(refraction[1] == Approx(sinf(M_PI / 6.0f)).margin(0.03f));
    REQUIRE(refraction[2] == Approx(-cosf(M_PI / 6.0f)).margin(0.03f));
    REQUIRE(!isTotalInternalReflection);

    refraction = refractionDir(refraction, multiply(normal, -1), 1.5f, isTotalInternalReflection);
    REQUIRE(refraction[0] == Approx(rayDirection[0]));
    REQUIRE(refraction[1] == Approx(rayDirection[1]));
    REQUIRE(refraction[2] == Approx(rayDirection[2]));
    REQUIRE(!isTotalInternalReflection);
}


TEST_CASE("Total internal reflection") {
    Vec3f rayDirection = normalize(Vec3f({ 0, sinf(M_PI / 4.0f), -cosf(M_PI / 4.0f) }));
    Vec3f normal({ 0, 0, -1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    REQUIRE(isTotalInternalReflection);

    rayDirection = normalize(Vec3f({ 0, sinf(M_PI / 3.0f), -cosf(M_PI / 3.0f) }));
    isTotalInternalReflection = false;
    refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    REQUIRE(isTotalInternalReflection);
}


TEST_CASE("Just before total internal reflection") {
    Vec3f rayDirection = normalize(Vec3f({ 0, sinf(M_PI / 4.5f), -cosf(M_PI / 4.5f) }));
    Vec3f normal({ 0, 0, -1 });
    bool isTotalInternalReflection;
    Vec3f refraction = refractionDir(rayDirection, normal, 1.5f, isTotalInternalReflection);
    REQUIRE(!isTotalInternalReflection);
}
