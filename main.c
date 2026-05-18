#define _XOPEN_SOURCE_EXTENDED 1
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <wchar.h>

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

#define nitems(arr) (sizeof(arr) / sizeof(arr[0]))

#pragma endregion

#pragma region GENERATION

void fillTile(int x, int y) {
    float num = rand() / (double)RAND_MAX;
    for (int i = 0; i < GRID_RES; i++) {
        for (int n = 0; n < GRID_RES; n++) {
            map[idx(x*GRID_RES + i, y*GRID_RES + n, SIZE*GRID_RES)] = (int)(num > 0.7 || x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1);
        }
    }
}

#pragma endregion

#pragma region DRAWING

void drawBuf(int buf[], int len, int xBound) {
    for (int y = 0; y < len / xBound; y += 3) {
        for (int x = 0; x < xBound; x += 2) {
            cchar_t ch;
            wchar_t br = (0x2800 + (buf[idx(x, y, xBound)] << 5) | (buf[idx(x, y+1, xBound)] << 4) | (buf[idx(x, y+2, xBound)] << 3) | (buf[idx(x+1, y, xBound)] << 2) | (buf[idx(x+1, y+1, xBound)] << 1) | buf[idx(x+1, y+2, xBound)]);
            setcchar(&ch, &br, 0, 0, NULL);
        }
        printw("%s", "\n");
    }
}

#pragma endregion

#pragma region MAIN LOOP

struct vec2 pos;
int yRot = 0;

void raycast(int proj[], size_t size) {
    memset(proj, 0, size);
    for (int rot = -45; rot < 45; rot++) {
        struct vec2 rayPos = pos;
        struct vec2 dir = rotVec(degToRad((rot+yRot)%360));
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

void movePlayer(int rot) {
    struct vec2 rVec = rotVec(yRot);
    struct vec2 nPos = pos;
    nPos.x += round(rVec.x);
    nPos.y += round(rVec.y);
    if (map[idx((int)nPos.x, (int)nPos.y, SIZE*GRID_RES)] == 0)
        pos = nPos;
}

int buf[90*HEIGHT] = {0};
int last[90*HEIGHT];

int main() {
    srand(time(NULL));

    pos.x = 20;
    pos.y = 16;

    char key;

    setlocale(LC_ALL, "");

    initscr();
    raw();
    noecho();
    nodelay(stdscr, TRUE);

    start_color();
    use_default_colors();

    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            fillTile(x, y);
        }
    }

    while (key != '`') {
        key = getch();

        if (key == 'w') {
            movePlayer(yRot);
        }
        if (key == 's')
            movePlayer(yRot+180);
        if (key == 'd')
            movePlayer(yRot+90);
        if (key == 'a')
            movePlayer(yRot-90);
        
        if (key == 'e')
            yRot = (yRot + 5) % 360;
        if (key == 'q')
            yRot = (yRot - 5) % -360;

        raycast(buf, sizeof(buf));

        if (memcmp(buf, last, sizeof(buf)) != 0) {
            clear();

            printw("%f%s%f%s", pos.x, ", ", pos.y, "\n");
            printw("%d%s", yRot, "\n");

            drawBuf(buf, nitems(buf), 90);

            memcpy(last, buf, sizeof(buf));

            refresh();
        }
    }

    endwin();

    return 0;
}

#pragma endregion