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


#include <math.h>
#include "raylib.h"
#include "scene/scenemanager.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "coord.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include "player.h"

// Make raygui use our own mouse and keyboard functions
#define IsMouseButtonReleased mouseReleased
#define IsMouseButtonPressed mouseReleased
#define IsKeyReleased keyboardReleased
#define IsKeyPressed keyboardReleased

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#define NOB_IMPLEMENTATION
#include "nob.h"

#define TARGET_FPS 60
#ifdef DEBUG
    #define TARGET_TPS 60
#else
    #define TARGET_TPS 240
#endif

#define TIME_SCALE 1.0

#ifdef DEBUG
    #define TARGET_TPS_DEBUG_TOGGLE 15
    #define TIME_SCALE_DEBUG_TOGGLE 0.25
#endif

static void update(const double deltaTime);
static void updateUI();
static void draw();

long tps = 0;
long targetTps = TARGET_TPS;
double timeScale = TIME_SCALE;

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
    sceneswitcherTransitionEx(scenemanager->state, SCENE_LVLED, 1.0, 0.5);

    #ifdef DEBUG
        SetTraceLogLevel(LOG_ALL);
    #else
        SetTraceLogLevel(LOG_INFO);
    #endif

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
            update(timeSinceLastUpdate * timeScale);
            #ifdef DEBUG
                if (keyboardPressed(KEY_G))
                    timeScale = timeScale == TIME_SCALE ? TIME_SCALE_DEBUG_TOGGLE : TIME_SCALE;
                if (keyboardPressed(KEY_F))
                    targetTps = targetTps == TARGET_TPS ? TARGET_TPS_DEBUG_TOGGLE : TARGET_TPS;
            #endif
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

    scenemanagerDestroy(scenemanager);

    CloseWindow();

    return 0;
}


static void update(const double deltaTime) {
    keyboardUpdate();
    mouseUpdate();
    
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

        #ifdef DEBUG
            static const char* debugModeText = "DEBUG MODE";
            int y = 10;
            DrawText(debugModeText, GetScreenWidth() - MeasureText(debugModeText, 48) - 10 + 2, y + 2, 48, BLACK);
            DrawText(debugModeText, GetScreenWidth() - MeasureText(debugModeText, 48) - 10, y, 48, RED);
            y += 48;
            const char* instruction1Text = TextFormat("Press G to set the time scale to %.02f", timeScale == TIME_SCALE ? TIME_SCALE_DEBUG_TOGGLE : TIME_SCALE);
            DrawText(instruction1Text, GetScreenWidth() - MeasureText(instruction1Text, 24) - 10 + 1, y + 1, 24, BLACK);
            DrawText(instruction1Text, GetScreenWidth() - MeasureText(instruction1Text, 24) - 10, y, 24, LIGHTGRAY);
            y += 24;
            const char* instruction2Text = TextFormat("Press F to set the target TPS to %d", targetTps == TARGET_TPS ? TARGET_TPS_DEBUG_TOGGLE : TARGET_TPS);
            DrawText(instruction2Text, GetScreenWidth() - MeasureText(instruction2Text, 24) - 10 + 1, y + 1, 24, BLACK);
            DrawText(instruction2Text, GetScreenWidth() - MeasureText(instruction2Text, 24) - 10, y, 24, LIGHTGRAY);
        #endif

    EndDrawing();
}