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


Scene scene(6, 4);


void handleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(
        1, 1, 0,
        0, 0, 0,
        0, 1, 0
    );

    glBegin(GL_LINES);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, -2);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 2);
        glColor3f(1, 0, 0);
        scene.render();
        glutWireCube(0.5);
    glEnd();

    glFlush();
    glutPostRedisplay();
}


void handleReshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float) w / (float) h, 0.01f, 100);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 400);
    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(handleDisplay);
    glutReshapeFunc(handleReshape);

    glutMainLoop();

    return 0;
}
