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
#define NOB_IMPLEMENTATION
#include "nob.h"
#include "scene/scenemanager.h"
#include "keyboard.h"
#include "dynamicarrays.h"
#include "coord.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include "player.h"
#include <math.h>

#define TARGET_FPS 60
#define TARGET_TPS 240

#define TIME_SCALE 1.0

static void update(const double deltaTime);
static void draw();

long tps = 0;

SceneManager* scenemanager;

int main(void) {
    // Initialize the window and make it resizable
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(800, 600);
    // Disable raylib exiting the game when you press ESCAPE
    SetExitKey(KEY_NULL);

    // We don't need to set the target FPS, because we limit the amount of updates and draw calls already

    // Initialize the Scene Manager
    scenemanager = scenemanagerCreate();
    scenemanagerLoad(scenemanager, SCENE_LEVEL);

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
            tps = roundl(1.0 / timeSinceLastUpdate);
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
    keyboardUpdate();

    scenemanagerUpdate(scenemanager, deltaTime);
}

static void draw() {
    BeginDrawing();

        scenemanagerDraw(scenemanager);

    EndDrawing();
}