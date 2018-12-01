#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <ctime>

#include "Renderer.h"
#include "Scene.h"


#define CANVAS_WIDTH 600
#define CANVAS_HEIGHT 500


Scene scene;
Renderer renderer(
    scene,
    CANVAS_WIDTH,
    CANVAS_HEIGHT,
    3,
    4,
    RANDOM,
    true,
    20,
    12
);


void handleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(
        0, 0, 0,
        0, 0, -1,
        0, 1, 0
    );

    scene.drawObjectsGL();

    glFlush();
}


// TODO: For future raytracing rendering
void setOrthographicProjection(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Center the statically-sized canvas within the window. Essentially we're
    // computing the correct margin between the sides of the window and when
    // (0,0) to (canvasWidth, canvasHeight) is centered.
    float centeringMarginX = (float) (w - CANVAS_WIDTH) / 2.0f;
    float centeringMarginY = (float) (h - CANVAS_HEIGHT) / 2.0f;
    gluOrtho2D(
        -centeringMarginX,
        w - centeringMarginX,
        -centeringMarginY,
        h - centeringMarginY
    );

    glMatrixMode(GL_MODELVIEW);
}


void handleReshape(int w, int h) {
    scene.setPerspectiveProjectionGL(w, h);
}


void handleKeyboard(unsigned char key, int _x, int _y) {
    if (key == 'q' || key == 'Q') {
        exit(0);
    }
}


int main(int argc, char **argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitWindowSize(CANVAS_WIDTH, CANVAS_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Raytracer Sandbox (Real-time Approximation Mode)");

    glutDisplayFunc(handleDisplay);
    glutKeyboardFunc(handleKeyboard);
    glutReshapeFunc(handleReshape);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    renderer.render();

    glutMainLoop();

    return 0;
}
