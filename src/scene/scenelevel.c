#include "scenelevel.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "raylib.h"
#include "stb_ds.h"
#include "input/keyboard.h"
#include "ground.h"
#include "level/level.h"

// #define STRESS_TEST

SceneLevel* scenelevelCreate() {
    SCENE_CREATE(SceneLevel);

    // Initialize the dynamic array of objects, the player and the camera, and colors
    this->objects = NULL;
    this->player = (Player){
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
    this->camera = (GDFCamera){0};
    this->levelSettings.backgroundColor = GetColor(0x287dffff);
    this->levelSettings.groundColor = GetColor(0x0066ffff);

#ifdef DEBUG
    // Add a test block to the objects DA
    // Object testBlock = {
    //     .position = {
    //         .x = 375,
    //         .y = 195 -5.5, // It should be just barely possible to jump under this, -6 should kill you
    //     },
    //     .angle = 0,
    //     .scale = 1,
    //     .id = 1,
    // };
    // arrput(this->objects, testBlock);
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
        arrput(this->objects, spike);
    }
    // Add another spike that should make it just barely possible to jump over the spikes
    // Object lastSpike = {
    //     .position = {
    //         .x = 405 +13, // +13.5 should be impossible
    //         .y = 105,
    //     },
    //     .angle = 0,
    //     .scale = 1,
    //     .id = 8,
    // };
    // arrput(this->objects, lastSpike);

    // Add stairs
    for (int i = 0; i < 100; i++) {
        Object block = {
            .position = {
                .x = 615 + i * 30 * 4,
                .y = 105 + i * 30,
            },
            .angle = 0,
            .scale = 1,
            .id = 1,
        };
        arrput(this->objects, block);
    }

    // Add platforms
    for (int i = 0; i < 100; i++) {
        for (int x = i * 900 + 885; x < i * 900 + 885 + 300; x += 30) {
            Object block = {
                .position = {
                    .x = x,
                    .y = 135,
                },
                .angle = 0,
                .scale = 1,
                .id = 1,
            };
            arrput(this->objects, block);
        }
    }

    #ifdef STRESS_TEST
        // Stress test
        for (int i = 0; i < 40000; i++) {
            Object block = {
                .position = {
                    .x = 990 + i * 30 * 5,
                    .y = 105,
                },
                .angle = 0,
                .scale = 1,
                .id = 1,
            };
            Object spike = {
                .position = {
                    .x = 990 + i * 30 * 5,
                    .y = 135,
                },
                .angle = 0,
                .scale = 1,
                .id = 8,
            };
            arrput(this->objects, block);
            arrput(this->objects, spike);
        }
    #else // STRESS_TEST
        if (!levelSaveToFile("debuglevel.json", this->levelSettings, this->objects))
            TraceLog(LOG_ERROR, "Couldn't save the debug level!");
    #endif
#else
    if (!levelLoadFromFile("level.json", &this->levelSettings, &this->objects))
        TraceLog(LOG_ERROR, "Couldn't load save!");
#endif

    return this;
}

void scenelevelDestroy(SceneLevel* scenelevel) {
    free(scenelevel);
}


void scenelevelUpdate(SceneLevel* this, SceneState* sceneState, double deltaTime) {
    // Don't update anything except the camera
    if (sceneState->transition.transitioning)
        goto updateCamera;

    // Update the player
    if (!this->frameStep || keyboardDown(KEY_Q) || keyboardPressed(KEY_P)) playerUpdate(&this->player, this->objects, deltaTime);

    // Update the camera
updateCamera:
    cameraUpdate(&this->camera, this->player, deltaTime);

    // Don't update anything except the camera
    if (sceneState->transition.transitioning)
        return;

    // Some key combinations that aren't handled by any of the other update loops
    if (keyboardPressed(KEY_R)) {
        playerReset(&this->player);
    }

    if (keyboardPressed(KEY_ESCAPE)) {
        this->frameStep = !this->frameStep;
    }

    if (keyboardPressedMod(KEY_ESCAPE, false, true)) {
        sceneswitcherTransitionTo(sceneState, SCENE_LVLED);
    }
}

void scenelevelUpdateUI(SceneLevel* this, SceneState* sceneState) {
    (void) this;
    (void) sceneState;
}

void scenelevelDraw(SceneLevel* this) {
    cameraRecalculateScreenSize(&this->camera);

    // Set the backgrond color
    ClearBackground(this->levelSettings.backgroundColor);
    // Temporary fix for a weird transparency issue
    DrawRectangle(0, 0, this->camera.screenSize.x, this->camera.screenSize.y, this->levelSettings.backgroundColor);

    // Draw the player
    playerDraw(this->player, this->camera);

    // Draw the objects
    size_t objectsLen = arrlenu(this->objects);
    for (size_t i = 0; i < objectsLen; ++i) {
        objectDraw(this->objects[i], true, this->camera);
    }

    // Draw the hitboxes
    for (size_t i = 0; i < objectsLen; ++i) {
        objectDrawHitbox(this->objects[i], false, this->camera);
    }
    playerDrawHitboxes(this->player, true, this->camera);

    // Draw the ground
    drawGround(this->levelSettings.groundColor, this->camera);
}
