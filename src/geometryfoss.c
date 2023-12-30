#include "raylib.h"
// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "coord.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include "player.h"
#include <stdio.h>

#define TARGET_FPS 60
#define TARGET_TPS 240

static void update(const double deltaTime);
static void draw();

typedef struct {
    Object* items;
    size_t count;
    size_t capacity;
} DAObjects;

DAObjects objects;
Player player;
GDFCamera camera;
Color backgroundColor;
Color groundColor;

int main(void) {
    // Initialize the window and make it resizable
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(800, 600);

    // We don't need to set the target FPS, because we limit the amount of updates and draw calls already

    // Initialize the dynamic array of objects, the player and the camera, and colors
    objects = (DAObjects){0};
    player = (Player){
        .position = {
            .x = 0,
            .y = 105,
        },
        .angle = 0,
        .outerHitbox = {
            .shape = SQUARE,
            .offset = {
                .x = 0,
                .y = 0,
            },
            .width = PLAYER_SIZE,
            .height = PLAYER_SIZE,
        },
        .innerHitbox = {
            .shape = SQUARE,
            .offset = {
                .x = 0,
                .y = 0,
            },
            .width = (double)PLAYER_SIZE / 3.0,
            .height = (double)PLAYER_SIZE / 3.0,
        },
    };
    camera = (GDFCamera){0};
    backgroundColor = GetColor(0x287dffff);
    groundColor = GetColor(0x0066ffff);

    // Add a test block to the objects DA
    Object testBlock = {
        .position = {
            .x = 135,
            .y = 105,
        },
        .angle = 0,
        .scale = 1,
        .id = 1,
    };
    nob_da_append(&objects, testBlock);

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
    cameraUpdate(&camera, player, deltaTime);
}

static void draw() {
    // Set the screen size of the camera
    camera.screenSize = (ScreenCoord){
        .x = GetScreenWidth(),
        .y = GetScreenHeight(),
    };
    // Convert the screen size to GD coordinates
    cameraConvertScreenSize(&camera);

    BeginDrawing();

        // Set the backgrond color
        ClearBackground(backgroundColor);

        drawGround(groundColor, camera);

        // Draw the objects
        for (size_t i = 0; i < objects.count; ++i) {
            objectDraw(objects.items[i], false, camera);
        }

        // Draw the player
        playerDraw(player, true, camera);

    EndDrawing();
}