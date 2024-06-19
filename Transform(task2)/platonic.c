
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define _PI 3.14159f

enum { TRANS, ROT, SCALE };
static int g_op_mode = TRANS;

enum { PERSP, ORTHO };
static int g_proj_mode = PERSP;

// window dimension
static int g_width = 800;
static int g_height = 800;

// angle (in degree) to rotate around x, y, z
static GLfloat g_angle[3] = { 0.0f, 0.0f, 0.0f };

// amount to translate along x, y, z
static GLfloat g_trans[3] = { 0.0f, 0.0f, 0.0f };

// scaling factor along x, y, z
static GLfloat g_scale[3] = { 1.0f, 1.0f, 1.0f };

// recursion level
static int g_level = 0;

// Model-View transforms
static void myTranslatef(GLfloat tx, GLfloat ty, GLfloat tz) {
    // Complete
    GLfloat m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        tx, ty, tz, 1.0f
    };

    glMultMatrixf(m);
}

static void myScalef(GLfloat sx, GLfloat sy, GLfloat sz) {
    // Complete
    GLfloat m[16] = {
        sx, 0.0f, 0.0f, 0.0f,
        0.0f, sy, 0.0f, 0.0f,
        0.0f, 0.0f, sz, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMultMatrixf(m);
}

static void myRotatef(GLfloat theta, GLfloat kx, GLfloat ky, GLfloat kz) {
    // Complete
    theta = theta * (_PI / 180);

    GLfloat m[16] = {
        kx * kx * (1 - cos(theta)) + cos(theta), kx * ky * (1 - cos(theta)) + kz * sin(theta), kx * kz * (1 - cos(theta)) - ky * sin(theta), 0.0f,
        ky * kx * (1 - cos(theta)) - kz * sin(theta), ky * ky * (1 - cos(theta)) + cos(theta), ky * kz * (1 - cos(theta)) + kx * sin(theta), 0.0f,
        kz * kx * (1 - cos(theta)) + ky * sin(theta), kz * ky * (1 - cos(theta)) - kx * sin(theta), kz * kz * (1 - cos(theta)) + cos(theta), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMultMatrixf(m);
}

// Projection transforms
static void myOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat n, GLfloat f) {
    // Complete

    GLfloat m[16] = {
        2 / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2 / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, 2 / (n - f), 0.0f,
        -((right + left) / (right - left)), -((top + bottom) / (top - bottom)), (f + n) / (n - f), 1.0f
    };

    glMultMatrixf(m);
}

static void myPerspective(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f) {
    // Complete
    fovy = fovy * (_PI / 180);
    GLfloat right = tan(fovy / 2) * n * aspect;
    GLfloat left = -tan(fovy / 2) * n * aspect;
    GLfloat top = tan(fovy / 2) * n;
    GLfloat bottom = -tan(fovy / 2) * n;

    GLfloat m[16] = {
        2 * n / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2 * n / (top - bottom), 0.0f, 0.0f,
        (right + left) / (right - left), (top + bottom) / (top - bottom), -(f + n) / (f - n), -1.0f,
        0.0f, 0.0f, -2 * f * n / (f - n), 0.0f
    };

    glMultMatrixf(m);
}

static void drawIcosahedron(void) {
    GLfloat phi = (1.f + sqrtf(5.f)) * .5f;
    GLfloat a = 1.f;
    GLfloat b = 1.f / phi;

    GLfloat vertices[12][3] = { {0.f,b,-a}, {b,a,0.f}, {-b,a,0.f},
    {0.f,b,a}, {0.f,-b,a}, {-a,0.f,b},
    {0.f,-b,-a}, {a,0.f,-b}, {a,0.f,b},
    {-a,0.f,-b},{b,-a,0.f},{-b,-a,0.f}
    };

    GLfloat color[20][3] = { {0.0f,0.0f,0.6f}, {0.0f,0.0f,0.8f}, {0.0f,0.0f,1.0f},
    {0.f, 0.2f, 1.f}, {0.f,0.4f,1.f}, {0.f,0.6f,1.f}, {0.f,0.8f,1.f}, {0.f,1.f,1.f},
    {0.2f,1.f,0.8f}, {0.4f,1.f,0.6f}, {0.6f,1.f,0.4f}, {0.8f,1.f,0.2f},
    {1.f,1.f,0.f}, {1.f,0.8f,0.f}, {1.f,0.6f,0.f}, {1.f,0.4f,0.f}, {1.f,0.2f,0.f},
    {1.f,0.f,0.f}, {0.8f,0.f,0.f}, {0.6f,0.f,0.f}
    };

    int faces[20][3] = { {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3},
    {7,6,0}, {6,9,0}, {11,10,4}, {10,11,6},
    {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
    {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0},
    {11,9,6}, {7,10,6}, {5,11,4}, {10,8,4}
    };

    int i;

    for (i = 0; i < 20; ++i) {
        GLfloat* c = color[i];
        int* f = faces[i];
        int v0 = f[0], v1 = f[1], v2 = f[2];
        glColor3f(c[0], c[1], c[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(vertices[v0][0], vertices[v0][1], vertices[v0][2]);
        glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
        glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
        glEnd();
    }
}

static void recursiveDrawIcosahedron(int level) {
    drawIcosahedron();

    if (level > 0) {
        glPushMatrix();
        //glTranslatef(1.0f, -1.5f, 0.0f);
        //glScalef(0.5f, 0.5f, 0.5f);
        myTranslatef(1.0f, -1.5f, 0.0f);
        myScalef(0.5f, 0.5f, 0.5f);
        recursiveDrawIcosahedron(level - 1);
        glPopMatrix();

        glPushMatrix();
        //glTranslatef(-1.0f, -1.5f, 0.0f);
        //glScalef(0.5f, 0.5f, 0.5f);
        myTranslatef(-1.0f, -1.5f, 0.0f);
        myScalef(0.5f, 0.5f, 0.5f);
        recursiveDrawIcosahedron(level - 1);
        glPopMatrix();
    }
    return;
}

static void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (g_proj_mode == PERSP) {
        // Complete: 
        // After completing the code of myPerspective() above, 
        // replace the call to gluPerspective with a call to myPerspective 
        // or gluPerspective depending on the transformation mode

        //gluPerspective(45.0, (GLdouble)g_width / (GLdouble)g_height, 0.1, 20.0);
        myPerspective(45.0, (GLdouble)g_width / (GLdouble)g_height, 0.1, 20.0);
    }
    else {
        // Complete: 
        // After completing the code of myOrtho() above, 
        // replace the call to glOrtho with a call to myOrtho
        // or glOrtho depending on the transformation mode

        //glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
        myOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
    }

    // Modelview transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glPushMatrix();

    // Complete: 
    // After completing the code of myTranslatef, myScalef, 
    // and myRotatef, 
    // replace the call to glTranslatef, glRotatef, glScalef by calls to 
    // myTranslatef, myScalef, and myRotatef or 
    // glTranslatef, glRotatef, glScalef based on the transformation mode.

    //glTranslatef(g_trans[0], g_trans[1], g_trans[2]);
    myTranslatef(g_trans[0], g_trans[1], g_trans[2]);


    //glRotatef(g_angle[0], 1.f, 0.f, 0.f);
    //glRotatef(g_angle[1], 0.f, 1.f, 0.f);
    //glRotatef(g_angle[2], 0.f, 0.f, 1.f);

    myRotatef(g_angle[0], 1.f, 0.f, 0.f);
    myRotatef(g_angle[1], 0.f, 1.f, 0.f);
    myRotatef(g_angle[2], 0.f, 0.f, 1.f);


    //glScalef(g_scale[0], g_scale[1], g_scale[2]);
    myScalef(g_scale[0], g_scale[1], g_scale[2]);

    drawIcosahedron();

    recursiveDrawIcosahedron(g_level);

    glPopMatrix();

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    g_width = w;
    g_height = h;
}

// Increase the rotation angle by amt around ax
static void rotate(int ax, GLfloat amt) {
    g_angle[ax] += amt;
}

// Increase the translation by amt along ax
static void translate(int ax, GLfloat amt) {
    g_trans[ax] += amt;
}

// Multiply the scaling factor by amt along ax
static void scale(int ax, GLfloat amt) {
    g_scale[ax] *= amt;
}

static void keyboard(unsigned char k, int x, int y) {
    switch (k) {
    case 27:
        exit(EXIT_SUCCESS);
        break;

        // Complete: 
        // Allow to switch between OpenGL transformations and your implementations

    case 'n':
        if (g_level < 5) ++g_level;
        break;
    case 'N':
        if (g_level > 0) --g_level;
        break;

    case 'p':
        g_proj_mode = (1 - g_proj_mode);
        break;

    case 't':
    case 'T':
        g_op_mode = TRANS;
        break;

    case 'r':
    case 'R':
        g_op_mode = ROT;
        break;

    case 's':
    case 'S':
        g_op_mode = SCALE;
        break;

    case 'x':
        if (g_op_mode == TRANS) translate(0, -0.5f);
        if (g_op_mode == ROT) rotate(0, -5.0f);
        if (g_op_mode == SCALE) scale(0, 0.9f);
        break;

    case 'X':
        if (g_op_mode == TRANS) translate(0, 0.5f);
        if (g_op_mode == ROT) rotate(0, 5.0f);
        if (g_op_mode == SCALE) scale(0, 1.1f);
        break;

    case 'y':
        if (g_op_mode == TRANS) translate(1, -0.5f);
        if (g_op_mode == ROT) rotate(1, -5.0f);
        if (g_op_mode == SCALE) scale(1, 0.9f);
        break;

    case 'Y':
        if (g_op_mode == TRANS) translate(1, 0.5f);
        if (g_op_mode == ROT) rotate(1, 5.0f);
        if (g_op_mode == SCALE) scale(1, 1.1f);
        break;

    case 'z':
        if (g_op_mode == TRANS) translate(2, -0.5f);
        if (g_op_mode == ROT) rotate(2, -5.0f);
        if (g_op_mode == SCALE) scale(2, 0.9f);
        break;

    case 'Z':
        if (g_op_mode == TRANS) translate(2, 0.5f);
        if (g_op_mode == ROT) rotate(2, 5.0f);
        if (g_op_mode == SCALE) scale(2, 1.1f);
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Icosahedron");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}