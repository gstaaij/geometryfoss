#include "scenemanager.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "raylib.h"
#include "ui/popup.h"

SceneManager* scenemanagerCreate() {
    SceneManager* scenemanager = (SceneManager*) malloc(sizeof(SceneManager));
    assert(scenemanager != NULL && "You don't have enough RAM");
    memset(scenemanager, 0, sizeof(SceneManager));

    scenemanager->state = sceneswitcherCreateState();

    return scenemanager;
}
void scenemanagerDestroy(SceneManager* scenemanager) {
    scenemanagerUnload(scenemanager);
    sceneswitcherDestroyState(scenemanager->state);
    free(scenemanager);
}

void scenemanagerLoad(SceneManager* scenemanager, const SceneEnum scene) {
    scenemanagerUnload(scenemanager);
    switch (scene) {
        case SCENE_NONE: {} break;
        case SCENE_LEVEL: {
            scenemanager->scenelevel = scenelevelCreate();
        } break;
        case SCENE_LVLED: {
            scenemanager->scenelvled = scenelvledCreate();
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene is not implemented");
        } break;
    }
    scenemanager->state->currentScene = scene;
}

void scenemanagerUnload(SceneManager* scenemanager) {
    switch (scenemanager->state->currentScene) {
        case SCENE_LEVEL: {
            scenelevelDestroy(scenemanager->scenelevel);
        } break;
        case SCENE_LVLED: {
            scenelvledDestroy(scenemanager->scenelvled);
        } break;
        default: {} break;
    }
    scenemanager->state->currentScene = SCENE_NONE;
    nob_temp_reset();
}

void scenemanagerUpdate(SceneManager* scenemanager, const double deltaTime) {
    switch (scenemanager->state->currentScene) {
        case SCENE_NONE: {} break;
        case SCENE_LEVEL: {
            scenelevelUpdate(scenemanager->scenelevel, scenemanager->state, deltaTime);
        } break;
        case SCENE_LVLED: {
            scenelvledUpdate(scenemanager->scenelvled, scenemanager->state, deltaTime);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene update loop is not implemented");
        } break;
    }

    sceneswitcherUpdate(scenemanager->state, deltaTime);
    if (sceneswitcherShouldLoadNewScene(scenemanager->state)) {
        scenemanagerLoad(scenemanager, scenemanager->state->transition.targetScene);
        sceneswitcherLoadedNewScene(scenemanager->state);
    }
}

void scenemanagerUpdateUI(SceneManager* scenemanager) {
    
    switch (scenemanager->state->currentScene) {
        case SCENE_NONE: {} break;
        case SCENE_LEVEL: {
            scenelevelUpdateUI(scenemanager->scenelevel, scenemanager->state);
        } break;
        case SCENE_LVLED: {
            scenelvledUpdateUI(scenemanager->scenelvled, scenemanager->state);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene update UI loop is not implemented");
        } break;
    }

    sceneswitcherUpdateUI(scenemanager->state);
}

void scenemanagerDraw(SceneManager* scenemanager) {
    switch (scenemanager->state->currentScene) {
        case SCENE_NONE: {
            ClearBackground(BLACK);
        } break;
        case SCENE_LEVEL: {
            scenelevelDraw(scenemanager->scenelevel);

            #ifdef DEBUG
                int y = 34 + 24;
                const char* labelTimeText = TextFormat("Time: %llf", scenemanager->scenelevel->player.timeAlive);
                DrawText(labelTimeText, 10, y, 24, WHITE);
                y += 24;
                const char* labelXText = TextFormat("X: %f", scenemanager->scenelevel->player.position.x);
                DrawText(labelXText, 10, y, 24, WHITE);
                y += 24;
                const char* labelYText = TextFormat("Y: %f", scenemanager->scenelevel->player.position.y);
                DrawText(labelYText, 10, y, 24, WHITE);
                y += 24;
                const char* labelVelXText = TextFormat("velX: %f", scenemanager->scenelevel->player.velocity.x);
                DrawText(labelVelXText, 10, y, 24, WHITE);
                y += 24;
                const char* labelVelYText = TextFormat("velY: %f", scenemanager->scenelevel->player.velocity.y);
                DrawText(labelVelYText, 10, y, 24, WHITE);
                y += 24;
                const char* labelOnGroundText = TextFormat("Grounded: %s", scenemanager->scenelevel->player.isOnGround ? "true" : "false");
                DrawText(labelOnGroundText, 10, y, 24, WHITE);
            #endif
        } break;
        case SCENE_LVLED: {
            scenelvledDraw(scenemanager->scenelvled);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene draw loop is not implemented");
        } break;
    }
}
