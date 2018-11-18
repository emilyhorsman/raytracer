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


TEST_CASE("Ray-Sphere Intersection", "hello") {
    Sphere s(zero, 0, 0, 0, Vec3f({ 0, 0, -1 }), 0.5f);
    Vec3f rayOrigin({ 0, 0, 0 });
    Vec3f rayDirection({ 0, 0, -1 });
    float scalar;
    REQUIRE(s.intersect(rayOrigin, rayDirection, scalar));
}

