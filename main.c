#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <curses.h>

struct vec2 {
    float x;
    float y;
};

#define SIZE 10
#define GRID_RES 4
#define HEIGHT 50

int map[SIZE*GRID_RES*SIZE*GRID_RES] = {0};

#pragma region UTILS

#define PI 3.141593

struct vec2 rotVec(float rot) {
    struct vec2 vec;
    vec.x = cos(rot);
    vec.y = sin(rot);
    return vec;
}

float degToRad(int deg) {
    return deg * PI / 180;
}

int idx(int x, int y, int xBound) {
    return x + y * xBound;
}

int min(int a, int b) {
    return (a < b ? a : b);
}

int max(int a, int b) {
    return (a > b ? a : b);
}

#pragma endregion

#pragma region GENERATION

void fillTile(int x, int y) {
    float num = rand() / (double)RAND_MAX;
    for (int i = 0; i < GRID_RES; i++) {
        for (int n = 0; n < GRID_RES; n++) {
            map[idx(x*GRID_RES + i, y*GRID_RES + n, SIZE*GRID_RES)] = (int)(/*num > 0.7 ||*/ x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1);
        }
    }
}

#pragma endregion

#pragma region DRAWING

void drawBuf(int buf[], int len, int xBound) {
    for (int y = 0; y < len / xBound; y += 2) {
        for (int x = 0; x < xBound; x++) {
            char* px1 = (buf[idx(x, y, xBound)] == 1 ? "47" : "40");
            char* px2 = (buf[idx(x, y+1, xBound)] == 1 ? "37" : "30");
            printf("\033[%s;%sm%s\033[0m", px1, px2, "▄");
        }
        printf("%s", "\n");
    }
}

#pragma endregion

struct vec2 pos;

void raycast(int* proj) {
    for (int rot = -45; rot < 45; rot++) {
        struct vec2 rayPos = pos;
        struct vec2 dir = rotVec(degToRad(rot));
        while (map[idx((int)round(rayPos.x), (int)round(rayPos.y), SIZE*GRID_RES)] == 0) {
            rayPos.x += dir.x;
            rayPos.y += dir.y;
        }
        int dist = (int)sqrt(pow(rayPos.x-pos.x, 2) + pow(rayPos.y-pos.y, 2));
        for (int i = min(HEIGHT, dist); i < max(0, HEIGHT-dist); i++) {
            proj[idx(rot+45, i, 90)] = 1;
        }
    }
}

int main() {
    srand(time(NULL));

    pos.x = 20;
    pos.y = 16;

    int key;

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    while (key != 's') {
        key = getch();

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                fillTile(x, y);
            }
        }

        int buf[90*HEIGHT] = {0};
        raycast(buf);

        printf("%s", "\n");
        drawBuf(buf, sizeof(buf) / sizeof(int), 90);
        printf("%s", "\n");
    }

    return 0;
}