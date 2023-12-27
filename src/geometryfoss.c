#include "raylib.h"

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
    BeginDrawing();

        ClearBackground(BLACK);

        DrawCircle(1280/2, 720/2, 100, RED);

    EndDrawing();
}