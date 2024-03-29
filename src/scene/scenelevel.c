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
    SceneLevel* scenelevel = (SceneLevel*) malloc(sizeof(SceneLevel));
    assert(scenelevel != NULL && "You don't have enough RAM");
    memset(scenelevel, 0, sizeof(SceneLevel));

    // Initialize the dynamic array of objects, the player and the camera, and colors
    scenelevel->objects = NULL;
    scenelevel->player = (Player){
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
    scenelevel->camera = (GDFCamera){0};
    scenelevel->levelSettings.backgroundColor = GetColor(0x287dffff);
    scenelevel->levelSettings.groundColor = GetColor(0x0066ffff);

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
    // arrput(scenelevel->objects, testBlock);
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
        arrput(scenelevel->objects, spike);
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
    // arrput(scenelevel->objects, lastSpike);

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
        arrput(scenelevel->objects, block);
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
            arrput(scenelevel->objects, block);
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
            arrput(scenelevel->objects, block);
            arrput(scenelevel->objects, spike);
        }
    #else // STRESS_TEST
        if (!levelSaveToFile("debuglevel.json", scenelevel->levelSettings, scenelevel->objects))
            TraceLog(LOG_ERROR, "Couldn't save the debug level!");
    #endif
#else
    if (!levelLoadFromFile("level.json", &scenelevel->levelSettings, &scenelevel->objects))
        TraceLog(LOG_ERROR, "Couldn't load save!");
#endif

    return scenelevel;
}
void scenelevelDestroy(SceneLevel* scenelevel) {
    free(scenelevel);
}


void scenelevelUpdate(SceneLevel* scenelevel, SceneState* sceneState, double deltaTime) {
    // Don't update anything except the camera
    if (sceneState->transition.transitioning)
        goto updateCamera;

    // Update the player
    if (!scenelevel->frameStep || keyboardDown(KEY_Q) || keyboardPressed(KEY_P)) playerUpdate(&scenelevel->player, scenelevel->objects, deltaTime);

    // Update the camera
updateCamera:
    cameraUpdate(&scenelevel->camera, scenelevel->player, deltaTime);

    // Don't update anything except the camera
    if (sceneState->transition.transitioning)
        return;

    // Some key combinations that aren't handled by any of the other update loops
    if (keyboardPressed(KEY_R)) {
        playerReset(&scenelevel->player);
    }

    if (keyboardPressed(KEY_ESCAPE)) {
        scenelevel->frameStep = !scenelevel->frameStep;
    }

    if (keyboardPressedMod(KEY_ESCAPE, false, true)) {
        sceneswitcherTransitionTo(sceneState, SCENE_LVLED);
    }
}

void scenelevelUpdateUI(SceneLevel* scenelevel, SceneState* sceneState) {
    (void) scenelevel;
    (void) sceneState;
}

void scenelevelDraw(SceneLevel* scenelevel) {
    cameraRecalculateScreenSize(&scenelevel->camera);

    // Set the backgrond color
    ClearBackground(scenelevel->levelSettings.backgroundColor);
    // Temporary fix for a weird transparency issue
    DrawRectangle(0, 0, scenelevel->camera.screenSize.x, scenelevel->camera.screenSize.y, scenelevel->levelSettings.backgroundColor);

    // Draw the player
    playerDraw(scenelevel->player, scenelevel->camera);

    // Draw the objects
    size_t objectsLen = arrlenu(scenelevel->objects);
    for (size_t i = 0; i < objectsLen; ++i) {
        objectDraw(scenelevel->objects[i], scenelevel->camera);
    }

    // Draw the hitboxes
    for (size_t i = 0; i < objectsLen; ++i) {
        objectDrawHitbox(scenelevel->objects[i], true, scenelevel->camera);
    }
    playerDrawHitboxes(scenelevel->player, true, scenelevel->camera);

    // Draw the ground
    drawGround(scenelevel->levelSettings.groundColor, scenelevel->camera);
}
