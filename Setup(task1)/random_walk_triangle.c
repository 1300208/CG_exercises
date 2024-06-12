#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define N 10000

struct Points {
    float x;
    float y;
};

static struct Points getPoint(struct Points, struct Points*);

static void display(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Repaint the background with the color 
    // specified by glClearColor()
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(5.f);

    // Triangles
    struct Points p1, p2, p3;
    p1.x = 0; p1.y = 0;
    p2.x = 1; p2.y = 0;
    p3.x = 0.5; p3.y = (float)sqrt(3)/2;
    struct Points points[3] = {p1, p2, p3};

    glColor3f(0, 1, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glVertex2f(p3.x, p3.y);
    glEnd();

    struct Points currectPoint = points[rand() % 3];

    for (int i = 0; i <= N; i++) {
        struct Points mp = getPoint(currectPoint, points);

        glColor3f(1, 0, 0);
        glBegin(GL_POINTS);
        glVertex2f(mp.x, mp.y);
        glEnd();

        currectPoint = mp;
    }

    glutSwapBuffers();
}


static void initGL(void) {
    // Set the color for painting the background (White)
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Set an orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}


static void keyHandle(unsigned char key, int x, int y) {
    // Exit when ESC is pressed
    if (key == 27) exit(0);
}

static struct Points getPoint(struct Points currectPoint, struct Points points[]) {
    struct Points mp;
    struct Points random = points[rand() % 3];
    mp.x = (currectPoint.x + random.x) / 2;
    mp.y = (currectPoint.y + random.y) / 2;
    return mp;
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Setup");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyHandle);
    glutMainLoop();
    return 0;
}