#include "raylib.h"
#include "raymath.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#define WIDTH 1600
#define HEIGHT WIDTH 
#define FPS 60

int abs(int a) {
    return a < 0 ? -a : a;
}

void find(int SZ, int *width, int *height) {
    int T = SZ * 8;
    *width = T / 8;
    *height = 8 * (int)ceil(sqrt((double)(T) / 8.0 / 8.0));
    while ((*width) * (*height) < T) {
        (*width)++;
    }
}

int main(void) {
    FILE *file = fopen("./main.c", "rb");
    assert(file != NULL);
    fseek(file, 0L, SEEK_END);
    int sz = ftell(file);
    rewind(file);
    assert(sz != 0);
    // char text[sz];
    // assert(text != NULL);
    // int s = fread(text, sizeof *text, sz, file);
    // assert(s == sz);
    char *text = "Hello, how are you today???";
    assert(fclose(file) == 0);
    const int len = strlen(text);
    int width, height;
    find(len, &width, &height);
    const float rw = ceil((float)WIDTH / (float)width);
    const float rh = ceil((float)HEIGHT / (float)height);
    const float rs = max(rw, rh);
    int count = 0;
    for (int i = 0; i < len; ++i) {
        for (int bit = 0; bit < 8; ++bit) {
            int val = (text[i] >> bit) & 1;
            count += val == 1;
        }
    }
    Vector2 *positions = calloc(count, sizeof *positions);
    assert(positions != NULL);
    Vector2 *velocities = calloc(count, sizeof *velocities);
    assert(velocities != NULL);
    int x = 0, y = 0;
    int col = 0;
    int idx = 0;
    for (int i = 0; i < len; ++i) {
        char curr = text[i];
        x = 8 * col;
        for (int bit = 0; bit < 8; ++bit) {
            int val = (curr >> bit) & 1;
            if (val) {
                positions[idx] = (Vector2){x*rs+rs/2, y*rs+rs/2};
                Vector2 v = {i/(bit+1), bit * 50};
                velocities[idx] = v;
                idx++;
            }
            x++;
        }
        y++;
        if (y >= height) {
            y = 0;
            col++;
        }
    }
    int direction = -1;
    int frameCount = 0;
    InitWindow(WIDTH, HEIGHT, "Hello!");
    SetTargetFPS(FPS);
    while (!WindowShouldClose()) {
        const float dt = 0.01; //GetFrameTime();
        if (IsKeyReleased(KEY_SPACE)) {
            Image i = LoadImageFromScreen();
            assert(ExportImage(i, "./image.png"));
            UnloadImage(i);
        }
        BeginDrawing();
        ClearBackground(WHITE);
        for (idx = 0; idx < count; idx++) {
            Vector2 pos = positions[idx];
            Vector2 vel = velocities[idx];
            // DrawCircle(pos.x, pos.y, rs/2, BLACK);
            DrawRectangle(pos.x, pos.y, rs/2, rs/2, BLACK);
            pos = Vector2Add(pos, Vector2Scale(vel, direction * dt));
            float x = pos.x;
            float y = pos.y;
            if (x > WIDTH) x -= WIDTH;
            else if (x < 0) x += WIDTH;
            if (y > HEIGHT) y -= HEIGHT;
            else if (y < 0) y += HEIGHT;
            positions[idx] = (Vector2){x,y};
        }
        EndDrawing();
        if (frameCount++ % FPS == 0) direction *= -1;
    }
    return 0;
}