#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include "Scene.h"


#define CANVAS_WIDTH 600
#define CANVAS_HEIGHT 500


Scene scene(CANVAS_WIDTH, CANVAS_HEIGHT);


void handleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glBegin(GL_POINTS);
        scene.render();
    glEnd();

    glFlush();
}


void handleReshape(int w, int h) {
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


void handleKeyboard(unsigned char key, int _x, int _y) {
    if (key == 'q' || key == 'Q') {
        exit(0);
    }
}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(CANVAS_WIDTH, CANVAS_HEIGHT);
    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(handleDisplay);
    glutKeyboardFunc(handleKeyboard);
    glutReshapeFunc(handleReshape);

    glClearColor(0.3, 0.3, 0.3, 1);

    glutMainLoop();

    return 0;
}
