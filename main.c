#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
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

#define WIDTH 90
#define HEIGHT 50

int options[] = {
    0,
    0,
    0
};

int opmax[] = {
    2,
    1,
    1
};

char mnames[][8] = {
    "Renderer",
    "Font",
    "Quit"
};

int map[SIZE*GRID_RES*SIZE*GRID_RES] = {0};

#pragma region UTILS

#define PI 3.141593

struct vec2 rotVec(float rot) {
    struct vec2 vec;
    vec.x = cos(rot);
    vec.y = sin(rot);
    return vec;
}

float degToRad(float deg) {
    return deg * PI / 180;
}

int idx(int x, int y, int xBound) {
    return (x % xBound) + y * xBound;
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
            map[idx(x*GRID_RES + i, y*GRID_RES + n, SIZE*GRID_RES)] = (int)(/*num > 0.7 ||*/ x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1);
        }
    }
}

#pragma endregion

#pragma region DRAWING

void drawnbuf(int buf[], size_t len, int xBound) {
    for (int y = 0; y < len / xBound; y += 2) {
        for (int x = 0; x < xBound; x++) {
            int pair = COLOR_PAIR((int)(buf[idx(x, y, xBound)] == 1) + (int)(buf[idx(x, y+1, xBound)] == 1)*2 + 1);
            attron(pair);
            printw("%s", "▄");
            attroff(pair);
        }
        printw("%s", "\n");
    }
}

void drawbbuf(int buf[], size_t len, int xBound) {
    cchar_t ch;
    for (int y = 0; y < len / xBound; y += 4) {
        for (int x = 0; x < xBound; x += 2) {
            wchar_t br = (0x2800 + (buf[idx(x, y, xBound)] | (buf[idx(x, y+1, xBound)] << 1) | (buf[idx(x, y+2, xBound)] << 2) | (buf[idx(x+1, y, xBound)] << 3) | (buf[idx(x+1, y+1, xBound)] << 4) | (buf[idx(x+1, y+2, xBound)] << 5) | (buf[idx(x, y+3, xBound)] << 6) | (buf[idx(x+1, y+3, xBound)] << 7)));
            setcchar(&ch, &br, 0, 0, NULL);
            add_wch(&ch);
        }
        printw("%s", "\n");
    }
}

void drawwbuf(int buf[], size_t len, int xBound) {
    for (int y = 0; y < len / xBound; y++) {
        for (int x = 0; x < xBound; x++) {
            printw("%s", (buf[idx(x, y, xBound)] == 1 ? "██" : "  "));
        }
        printw("%s", "\n");
    }
}

#define drawBuf(buf, len, xBound) (options[0] == 0 ? drawnbuf(buf, len, xBound) : (options[0] == 1 ? drawbbuf(buf, len, xBound) : drawwbuf(buf, len, xBound)))

#pragma endregion

#pragma region DRAW METHODS

void blitrect(int buf[], int minx, int miny, int maxx, int maxy, int xBound, int ch) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            buf[idx(x, y, xBound)] = ch;
        }
    }
}

int FONT[8028];
int curfont = -1;

void loadfont() {
    char fp[] = { (options[1]+(char)'0'), '.', 't', 'x', 't' };
    FILE *font = fopen("0.txt", "r");

    fseek(font, 0l, SEEK_END);
    int len = ftell(font)/sizeof(char);

    rewind(font);

    char stream[len];
    fread(stream, sizeof(char), len, font);
    fclose(font);

    int chi = 0;
    for (int i = 0; i < len; i++) {
        if (stream[i] == '0' || stream[i] == '1') {
            FONT[chi] = stream[i] - '0';
            chi++;
        }
    }
}

void blitstr(int buf[], int x, int y, int xBound, char str[], size_t len) {
    for (int i = 0; i < len; i++) {
        for (int l = 0; l < 6; l++) {
            for (int r = 0; r < 6; r++) {
                buf[idx(x+i*6+r, y+l, xBound)] = FONT[((int)(str[i])-32)*36 + l*6 + r];
            }
        }
    }
}

#pragma endregion

#pragma region MAIN LOOP

struct vec2 pos;
int yRot = 0;

void raycast(int proj[]) {
    for (int rot = -WIDTH / 2; rot < WIDTH / 2; rot++) {
        struct vec2 rayPos = pos;
        struct vec2 dir = rotVec(degToRad((rot+yRot)%360));
        while (map[idx((int)round(rayPos.x), (int)round(rayPos.y), SIZE*GRID_RES)] == 0) {
            rayPos.x += dir.x;
            rayPos.y += dir.y;
        }
        int dist = (int)(sqrt(pow(rayPos.x-pos.x, 2) + pow(rayPos.y-pos.y, 2)));
        for (int i = min(HEIGHT, dist); i < max(0, HEIGHT-dist); i++) {
            proj[idx(rot+WIDTH/2, i, WIDTH)] = 1;
        }
    }
}

void movePlayer(int rot) {
    struct vec2 rVec = rotVec(degToRad(rot));
    struct vec2 nPos = pos;
    nPos.x += round(rVec.x);
    nPos.y += round(rVec.y);
    if (map[idx((int)nPos.x, (int)nPos.y, SIZE*GRID_RES)] == 0)
        pos = nPos;
}

int buffer[WIDTH*HEIGHT] = {0};
int last[WIDTH*HEIGHT];

bool menuopen = FALSE;

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
    keypad(stdscr, TRUE);

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

    while (!options[nitems(options)-1]) {
        if (curfont != options[1]) {
            loadfont();
            curfont = options[1];
        }

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
            yRot = (yRot + 355) % 360;
        
        if (key == '`') {
            menuopen = !menuopen;
        }
        
        size_t bsize = sizeof(buffer);

        memset(buffer, 0, bsize);
        raycast(buffer);

        if (menuopen) {
            blitrect(buffer, 8, 8, WIDTH-8, HEIGHT-8, WIDTH, 0);
            blitrect(buffer, 9, 9, WIDTH-9, HEIGHT-9, WIDTH, 1);
            blitrect(buffer, 10, 10, WIDTH-10, HEIGHT-10, WIDTH, 0);

            blitstr(buffer, 12, 12, WIDTH, "Menu", 4);

            for (int i = 0; i < nitems(options); i++) {
                blitstr(buffer, 12, 20+i*6, WIDTH, mnames[i], nitems(mnames[i]));

                char ch[1] = { options[i]+97 };
                blitstr(buffer, 18+nitems(mnames[i])*6, 20+i*6, WIDTH, ch, 1);

                if (key == i+'0') {
                    options[i] = (options[i] + 1) % (opmax[i]+1);
                }
            }
        }

        if (memcmp(buffer, last, bsize) != 0) {
            clear();

            drawBuf(buffer, nitems(buffer), WIDTH);

            memcpy(last, buffer, bsize);

            refresh();
        }
    }

    endwin();

    return 0;
}

#pragma endregion