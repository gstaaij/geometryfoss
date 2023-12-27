#include "raylib.h"
#include "screen.h"
#include <stdio.h>

static void update();
static void draw();

int main(void) {
    InitWindow(1280, 720, "Geometry FOSS");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    CloseWindow();

    return 0;
}


static void update() {

}

static void draw() {
    ScreenCoord screenSize = {
        .x = GetScreenWidth(),
        .y = GetScreenHeight(),
    };

    Coord screenSizeAsCoord = getScreenSizeAsCoord(screenSize.x, screenSize.y);
    Coord cameraCoord = {0};

    BeginDrawing();


        Color background = GetColor(0x287dffff);
        ClearBackground(background);

        Coord blockCoord = {
            .x = 15,
            .y = 15,
        };
        double blockSize = 30;

        ScreenCoord scBlock = getScreenCoord(blockCoord, cameraCoord, screenSizeAsCoord, screenSize);
        long scBlockSize = convertToScreen(blockSize, screenSizeAsCoord, screenSize);
        long scBlockLineThick = convertToScreen(1.5, screenSizeAsCoord, screenSize);

        Rectangle recBlock = {
            .x = scBlock.x - (scBlockSize / 2),
            .y = scBlock.y - (scBlockSize / 2),
            .width = scBlockSize,
            .height = scBlockSize,
        };
        DrawRectangleRec(recBlock, BLACK);
        DrawRectangleLinesEx(recBlock, scBlockLineThick, WHITE);

    EndDrawing();
}