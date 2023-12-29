#include "raylib.h"
#include "screen.h"
#include "object.h"
#include <stdio.h>

#define TARGET_FPS 60
#define TARGET_TPS 240

static void update(const double deltaTime);
static void draw();

int main(void) {
    // Initialize the window and make it resizable
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // We don't need to set the target FPS, because we limit the amount of updates and draw calls already

    // Initialize some variables that will be needed
    double timeSinceLastUpdate = 0;
    double timeSinceLastDraw = 0;
    double previousTime = 0;
    while (!WindowShouldClose()) {
        // Calculate the delta time
        double time = GetTime();
        double deltaTime = time - previousTime;
        previousTime = time;
        // Increase the time since we last ran update
        timeSinceLastUpdate += deltaTime;
        // Increase the time since we last ran draw
        timeSinceLastDraw += deltaTime;
        
        // If enough time has elapsed, update
        if (timeSinceLastUpdate >= 1.0/(double)TARGET_TPS) {
            update(timeSinceLastUpdate);
            timeSinceLastUpdate = 0;
        }

        // If enough time has elapsed, draw
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

    // Define the camera
    GDFCamera camera = {
        .position = {
            .x = 15,
            .y = 15,
        },
        .screenSizeAsCoord = {0},
        // Get the screen size
        .screenSize = {
            .x = GetScreenWidth(),
            .y = GetScreenHeight(),
        },
    };
    // Convert the screen size to GD coordinates
    getScreenSizeAsCoord(&camera);

    BeginDrawing();

        // Set the backgrond color
        Color background = GetColor(0x287dffff); // TODO: This shouldn't be happening every draw call
        ClearBackground(background);

        // Define our test block
        Object veryNiceBlock = {
            .position = {
                .x = 15,
                .y = 15,
            },
            .angle = 0,
            .scale = 1,
            .id = 1,
        };

        // Draw our test block without drawing the hitbox
        objectDraw(veryNiceBlock, false, camera);

    EndDrawing();
}