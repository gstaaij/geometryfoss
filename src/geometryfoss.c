// Geometry FOSS or GDF is an Open Source project that aims to be a good recreation of Geometry Dash
// Copyright (C) 2023-2024  gstaaij
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "raylib.h"
// #define NOB_IMPLEMENTATION
#include "nob.h"
#include "dynamicarrays.h"
#include "coord.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include "player.h"
#include <stdio.h>

#define TARGET_FPS 60
#define TARGET_TPS 240

#define TIME_SCALE 1.0

static void update(const double deltaTime);
static void draw();

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
        .velocity = {0},
        .angle = 0,
        .outerHitbox = {
            .shape = HITBOX_SQUARE,
            .offset = {
                .x = 0,
                .y = 0,
            },
            .width = PLAYER_SIZE,
            .height = PLAYER_SIZE,
        },
        .innerHitbox = {
            .shape = HITBOX_SQUARE,
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
            .x = 375,
            .y = 195 -5.5, // It should be just barely possible to jump under this, -6 should kill you
        },
        .angle = 0,
        .scale = 1,
        .id = 1,
    };
    nob_da_append(&objects, testBlock);
    // Add a triple spike
    for (size_t i = 345; i < 345+90; i += 30) {
        Object spike = {
            .position = {
                .x = i,
                .y = 105,
            },
            .angle = 0,
            .scale = 1,
            .id = 8,
        };
        nob_da_append(&objects, spike);
    }
    // Add another spike that should make it just barely possible to jump over the spikes
    Object lastSpike = {
        .position = {
            .x = 405 +13,
            .y = 105,
        },
        .angle = 0,
        .scale = 1,
        .id = 8,
    };
    nob_da_append(&objects, lastSpike);

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
            update(timeSinceLastUpdate * TIME_SCALE);
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
    // Update the player
    playerUpdate(&player, objects, deltaTime);
    // Update the camera
    cameraUpdate(&camera, player, deltaTime);

    // Some key combinations that aren't handled by any of the other update loops
    if (IsKeyPressed(KEY_R)) {
        playerReset(&player);
    }
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

        // Draw the player
        playerDraw(player, camera);

        // Draw the objects
        for (size_t i = 0; i < objects.count; ++i) {
            objectDraw(objects.items[i], true, camera);
        }

        // Draw the player hitboxes
        playerDrawHitboxes(player, true, camera);

        // Draw the ground
        drawGround(groundColor, camera);

    EndDrawing();
}