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
#include "SceneFile.h"


Scene scene;
Renderer renderer(scene);


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
    float centeringMarginX = (float) (w - renderer.mWidth) / 2.0f;
    float centeringMarginY = (float) (h - renderer.mHeight) / 2.0f;
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
    loadSceneFile(renderer, scene, "./sample.scene");

    glutInit(&argc, argv);
    glutInitWindowSize(renderer.mWidth, renderer.mHeight);
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
