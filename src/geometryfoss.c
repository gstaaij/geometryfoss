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
#include "player/player.h"
#include "assets/assets.h"
#include "assets/font.h"
#include "ui/text.h"

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

// 9 TPS is the minimum TPS where the physics function well enough for the player to die to things
#define MAX_DELTA_TIME 0.1112
// The amount of times in a row the delta time is allowed to be too high
#define MAX_DELTA_TIME_INCIDENTS 10

#ifdef DEBUG
    #define TARGET_TPS_DEBUG_TOGGLE 15
    #define TIME_SCALE_DEBUG_TOGGLE 0.25
#endif

static void update(const double deltaTime);
static void updateUI();
static void draw();

static bool showTpsAndFps = true;

static long tps = 0;
static long targetTps = TARGET_TPS;
static double timeScale = TIME_SCALE;

static int deltaTimeTooHighCounter = 0;

static SceneManager* scenemanager;

int main(void) {
    #ifdef DEBUG
        SetTraceLogLevel(LOG_ALL);
    #else
        SetTraceLogLevel(LOG_INFO);
    #endif

    // Initialize the window and make it resizable
    InitWindow(1280, 720, "Geometry FOSS");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(640, 480);
    // Disable raylib exiting the game when you press ESCAPE
    SetExitKey(KEY_NULL);

    // We don't need to set the target FPS, because we limit the amount of updates and draw calls already

    // Initialize the Scene Manager
    scenemanager = scenemanagerCreate();
    sceneswitcherTransitionEx(scenemanager->state, SCENE_LOAD_ASSETS, 1.0, 0.5);

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
        #ifndef DEBUG // If debug mode is enabled, don't limit the delta time
            if (timeSinceLastUpdate * timeScale <= MAX_DELTA_TIME) {
        #endif
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

                deltaTimeTooHighCounter = 0;
        #ifndef DEBUG
            } else {
                ++deltaTimeTooHighCounter;
                if (deltaTimeTooHighCounter > MAX_DELTA_TIME_INCIDENTS) {
                    // Immediately switch to the crash scene
                    sceneswitcherTransitionEx(scenemanager->state, SCENE_CRASH, 0.1, 0.1);
                    // Update so it actually switches
                    update(1.0 / targetTps);
                }
            }
        #endif

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

    assetsUnloadEverything();
    fontUnload();

    CloseWindow();

    return 0;
}

static void update(const double deltaTime) {  
    keyboardUpdate();
    mouseUpdate();
    fontUpdate();
    GuiSetFont(fontGetBig());

    scenemanagerUpdate(scenemanager, deltaTime);

    if (keyboardPressed(KEY_F3)) {
        showTpsAndFps = !showTpsAndFps;
    }
}

static void updateUI() {
    scenemanagerUpdateUI(scenemanager);
}

static void draw() {
    BeginDrawing();

        scenemanagerDraw(scenemanager);
        updateUI();

        if (showTpsAndFps) {
            // Display the FPS and TPS on the top left of the screen
            const int fs = 20;
            int y = 10;
            textDraw(GetFontDefault(), TextFormat("TPS: %ld", tps), 10, y, fs, WHITE);
            y += fs;
            textDraw(GetFontDefault(), TextFormat("FPS: %ld", GetFPS()), 10, y, fs, WHITE);
            y += 2 * fs;
            textDraw(GetFontDefault(), "Press F3 to toggle this text", 10, y, fs, WHITE);
            y += 2 * fs;
            if (scenemanager->state->currentScene == SCENE_LVLED) {
                textDraw(GetFontDefault(), "Drag the mouse around to move the view", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "Press CTRL+PLUS to zoom in and CTRL+MINUS to zoom out. CTRL+SCROLL can also be used", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "Press ESCAPE to enter the pause menu", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "In Build mode:    click to build something", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "In Edit mode:     click on objects to select them and use W, A, S and D to move them around", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "                     You can also use Q and E to rotate objects and select multiple objects by holding SHIFT", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "In Delete mode:  click on an object to remove it", 10, y, fs, WHITE);
                y += fs;
            } else if (scenemanager->state->currentScene == SCENE_LEVEL) {
                textDraw(GetFontDefault(), "Press SPACE or click the mouse to jump", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "Press ESCAPE to pause or unpause the game", 10, y, fs, WHITE);
                y += fs;
                textDraw(GetFontDefault(), "Press CTRL+ESCAPE to go back to the Editor", 10, y, fs, WHITE);
                y += fs;
            }
        }

        #ifdef DEBUG
            static const char* debugModeText = "DEBUG MODE";
            int y = 10;
            textDraw(GetFontDefault(), debugModeText, GetScreenWidth() - MeasureText(debugModeText, 48) - 10 + 2, y + 2, 48, BLACK);
            textDraw(GetFontDefault(), debugModeText, GetScreenWidth() - MeasureText(debugModeText, 48) - 10, y, 48, RED);
            y += 48;
            const char* instruction1Text = TextFormat("Press G to set the time scale to %.02f", timeScale == TIME_SCALE ? TIME_SCALE_DEBUG_TOGGLE : TIME_SCALE);
            textDraw(GetFontDefault(), instruction1Text, GetScreenWidth() - MeasureText(instruction1Text, 24) - 10 + 1, y + 1, 24, BLACK);
            textDraw(GetFontDefault(), instruction1Text, GetScreenWidth() - MeasureText(instruction1Text, 24) - 10, y, 24, LIGHTGRAY);
            y += 24;
            const char* instruction2Text = TextFormat("Press F to set the target TPS to %d", targetTps == TARGET_TPS ? TARGET_TPS_DEBUG_TOGGLE : TARGET_TPS);
            textDraw(GetFontDefault(), instruction2Text, GetScreenWidth() - MeasureText(instruction2Text, 24) - 10 + 1, y + 1, 24, BLACK);
            textDraw(GetFontDefault(), instruction2Text, GetScreenWidth() - MeasureText(instruction2Text, 24) - 10, y, 24, LIGHTGRAY);
        #endif

    EndDrawing();
}