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
#include "scene/scenemanager.h"
#include "coord.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include "player.h"
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#define NOB_IMPLEMENTATION
#include "nob.h"

#define TARGET_FPS 60
#define TARGET_TPS 240

#define TIME_SCALE 1.0

static void update(const double deltaTime);
static void updateUI();
static void draw();

double targetTps = TARGET_TPS;
long tps = 0;

SceneManager* scenemanager;

int main(void) {
    // Initialize the window and make it resizable
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(640, 480);
    // Disable raylib exiting the game when you press ESCAPE
    SetExitKey(KEY_NULL);

    // We don't need to set the target FPS, because we limit the amount of updates and draw calls already

    // Initialize the Scene Manager
    scenemanager = scenemanagerCreate();
    scenemanagerLoad(scenemanager, SCENE_LVLED);

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

        bool shouldDraw = timeSinceLastDraw >= 1.0/(double)TARGET_FPS;
        
        // If enough time has elapsed, update
        if (timeSinceLastUpdate >= 1.0/targetTps) {
            update(timeSinceLastUpdate * TIME_SCALE);
            if (!shouldDraw) {
                updateUI();
                PollInputEvents();
            }
            tps = roundl(1.0 / timeSinceLastUpdate);
            timeSinceLastUpdate = 0;
        }

        // If enough time has elapsed, draw
        if (shouldDraw) {
            draw();
            timeSinceLastDraw = 0;
        }
    }

    CloseWindow();

    return 0;
}


static void update(const double deltaTime) {
    // Make the TPS the same as the FPS when not in a level, to make the UI more consistent
    targetTps = scenemanager->currentScene == SCENE_LEVEL ? TARGET_TPS : TARGET_FPS;
    
    scenemanagerUpdate(scenemanager, deltaTime);
}

static void updateUI() {
    scenemanagerUpdateUI(scenemanager);
}

static void draw() {
    BeginDrawing();

        scenemanagerDraw(scenemanager);
        updateUI();

        // Display the FPS and TPS on the top left of the screen
        DrawText(TextFormat("TPS: %ld", tps), 10, 10, 24, WHITE);
        DrawText(TextFormat("FPS: %ld", GetFPS()), 10, 34, 24, WHITE);

    EndDrawing();
}