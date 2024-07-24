#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define the vertices and faces of an icosahedron
#define NUM_VERTICES 12
#define NUM_FACES 20

float phi;
float a;
float b;

// Vertices of the icosahedron
float vertices[NUM_VERTICES][3];

// Faces of the icosahedron
int faces[NUM_FACES][3] = {
    {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3},
    {7,6,0}, {6,9,0}, {11,10,4}, {10,11,6},
    {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
    {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0},
    {11,9,6}, {7,10,6}, {5,11,4}, {10,8,4}
};

void makeOFFfile(const char *filename) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "OFF\n");
    fprintf(file, "%d %d %d\n", NUM_VERTICES, NUM_FACES, 0);

    for (int i = 0; i < NUM_VERTICES; ++i) {
        fprintf(file, "%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
    }

    for (int i = 0; i < NUM_FACES; ++i) {
        fprintf(file, "3 %d %d %d\n", faces[i][0], faces[i][1], faces[i][2]);
    }

    fclose(file);
}

int main() {
    phi = (1.f + sqrtf(5.f)) * .5f;
    a = 1.f;
    b = 1.f / phi;
    vertices[0][0] = 0.f;
    vertices[0][1] = b;
    vertices[0][2] = -a;
    vertices[1][0] = b;
    vertices[1][1] = a;
    vertices[1][2] = 0.f;
    vertices[2][0] = -b;
    vertices[2][1] = a;
    vertices[2][2] = 0.f;
    vertices[3][0] = 0.f;
    vertices[3][1] = b;
    vertices[3][2] = a;
    vertices[4][0] = 0.f;
    vertices[4][1] = -b;
    vertices[4][2] = a;
    vertices[5][0] = -a;
    vertices[5][1] = 0.f;
    vertices[5][2] = b;
    vertices[6][0] = 0.f;
    vertices[6][1] = -b;
    vertices[6][2] = -a;
    vertices[7][0] = a;
    vertices[7][1] = 0.f;
    vertices[7][2] = -b;
    vertices[8][0] = a;
    vertices[8][1] = 0.f;
    vertices[8][2] = b;
    vertices[9][0] = -a;
    vertices[9][1] = 0.f;
    vertices[9][2] = -b;
    vertices[10][0] = b;
    vertices[10][1] = -a;
    vertices[10][2] = 0.f;
    vertices[11][0] = -b;
    vertices[11][1] = -a;
    vertices[11][2] = 0.f;

    makeOFFfile("icosahedron.off");
    printf("make icosahedron.off\n");
    return 0;
}