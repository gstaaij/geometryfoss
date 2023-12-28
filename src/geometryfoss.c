#include "raylib.h"
#include "screen.h"
#include "object.h"
#include <stdio.h>

#define TARGET_FPS 60
#define TARGET_TPS 240

static void update(const double deltaTime);
static void draw();

int main(void) {
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(TARGET_TPS);

    double timeSinceLastUpdate = 0;
    double timeSinceLastDraw = 0;
    double previousTime = 0;
    while (!WindowShouldClose()) {
        double time = GetTime();
        double deltaTime = time - previousTime;
        previousTime = time;
        timeSinceLastUpdate += deltaTime;
        timeSinceLastDraw += deltaTime;
        
        if (timeSinceLastUpdate >= 1.0/(double)TARGET_TPS) {
            update(timeSinceLastUpdate);
            timeSinceLastUpdate = 0;
        }

        if (timeSinceLastDraw >= 1.0/(double)TARGET_FPS) {
            draw();
            timeSinceLastDraw = 0;
        }
    }

    CloseWindow();

    return 0;
}


static void update(const double deltaTime) {
    (void) deltaTime;
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

        Object veryNiceBlock = {
            .position = {
                .x = 15,
                .y = 15,
            },
            .angle = 0,
            .scale = 1.5,
            .id = 1,
        };

        objectDraw(veryNiceBlock, true, cameraCoord, screenSizeAsCoord, screenSize);

    EndDrawing();
}