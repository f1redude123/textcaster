#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 10
#define RES 1

#define FOV 10
#define PREC 10
#define HEIGHT 50

#pragma region UTILS

#define PI 3.1415926535897932

double degToRad(double deg) {
    return deg * PI / 180.0;
}

#pragma endregion

#pragma region MOVEMENT

struct vec2 {
    int x;
    int y;
};

struct vec2 pos;

int rot = 0;

struct vec2 rotVec(double rot) {
    struct vec2 new;
    new.x = round(sin(rot));
    new.y = round(cos(rot));
    return new;
}

#pragma endregion

#pragma region GENERATION

int map[(SIZE*RES)*(SIZE*RES)];

int idx(int x, int y) {
    return x + y * SIZE * RES;
}

void fillTile(int x, int y, int c) {
    for (int j = 0; j < RES; j++) {
        for (int k = 0; k < RES; k++) {
            map[idx(x*RES+j, y*RES+k)] = c;
        }
    }
}

#pragma endregion

#pragma region DRAWING

int buffer[FOV*PREC*HEIGHT];

void draw() {
    for (double i = -FOV/PREC*2; i < FOV/PREC*2; i++) {
        struct vec2 rayPos = pos;
        struct vec2 dir = rotVec(degToRad(i));

        while (map[idx(dir.x, dir.y)] == 0) {
            rayPos.x += dir.x;
            rayPos.y += dir.y;
        }

        int dist = sqrt(pow(rayPos.x - pos.x, 2) + pow(rayPos.y, pos.y));

        buffer[idx((int)round(i + FOV/PREC*2), HEIGHT - dist)];
    }
}

void renderBuffer(int buf[], int xBound) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++) {
        if (i % xBound == 0 && i != 0) {
            putchar('\n');
        }
        printf("%s", buf[i] ? "██" : "  ");
    }
}

#pragma endregion

int main() {
    srand(time(NULL));

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            fillTile(x, y, (x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1 || rand() % 4 == 0));
        }
    }

    draw();
    renderBuffer(buffer, FOV*PREC);
}