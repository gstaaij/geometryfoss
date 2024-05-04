#include "scenemanager.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "raylib.h"
#include "ui/popup.h"
#include "input/keyboard.h"

SceneManager* scenemanagerCreate() {
    SCENE_CREATE(SceneManager);

    this->state = sceneswitcherCreateState();

    return this;
}
void scenemanagerDestroy(SceneManager* this) {
    scenemanagerUnload(this);
    sceneswitcherDestroyState(this->state);
    free(this);
}

void scenemanagerLoad(SceneManager* this, const SceneEnum scene) {
    scenemanagerUnload(this);
    switch (scene) {
        case SCENE_NONE: {} break;
        case SCENE_CRASH: {
            // This scene is very simple, we don't need a seperate structure for it
        } break;
        case SCENE_LOAD_ASSETS: {
            this->sceneloadassets = sceneloadassetsCreate();
        } break;
        case SCENE_LEVEL: {
            this->scenelevel = scenelevelCreate();
        } break;
        case SCENE_LVLED: {
            this->scenelvled = scenelvledCreate();
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene is not implemented");
        } break;
    }
    this->state->currentScene = scene;
}

void scenemanagerUnload(SceneManager* this) {
    switch (this->state->currentScene) {
        case SCENE_LOAD_ASSETS: {
            sceneloadassetsDestroy(this->sceneloadassets);
        } break;
        case SCENE_LEVEL: {
            scenelevelDestroy(this->scenelevel);
        } break;
        case SCENE_LVLED: {
            scenelvledDestroy(this->scenelvled);
        } break;
        default: {} break;
    }
    this->state->currentScene = SCENE_NONE;
    nob_temp_reset();
}

void scenemanagerUpdate(SceneManager* this, const double deltaTime) {
    switch (this->state->currentScene) {
        case SCENE_NONE: {} break;
        case SCENE_CRASH: {
            if (keyboardPressed(KEY_SPACE)) {
                /// TODO: make this return to the main menu
                sceneswitcherTransitionTo(this->state, SCENE_LVLED);
            }
        } break;
        case SCENE_LOAD_ASSETS: {
            sceneloadassetsUpdate(this->sceneloadassets, this->state, deltaTime);
        } break;
        case SCENE_LEVEL: {
            scenelevelUpdate(this->scenelevel, this->state, deltaTime);
        } break;
        case SCENE_LVLED: {
            scenelvledUpdate(this->scenelvled, this->state, deltaTime);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene update loop is not implemented");
        } break;
    }

    sceneswitcherUpdate(this->state, deltaTime);
    if (sceneswitcherShouldLoadNewScene(this->state)) {
        scenemanagerLoad(this, this->state->transition.targetScene);
        sceneswitcherLoadedNewScene(this->state);
    }
}

void scenemanagerUpdateUI(SceneManager* this) {
    
    switch (this->state->currentScene) {
        case SCENE_NONE: {} break;
        case SCENE_CRASH: {} break;
        case SCENE_LOAD_ASSETS: {
            sceneloadassetsUpdateUI(this->sceneloadassets, this->state);
        } break;
        case SCENE_LEVEL: {
            scenelevelUpdateUI(this->scenelevel, this->state);
        } break;
        case SCENE_LVLED: {
            scenelvledUpdateUI(this->scenelvled, this->state);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene update UI loop is not implemented");
        } break;
    }

    sceneswitcherUpdateUI(this->state);
}

void scenemanagerDraw(SceneManager* this) {
    switch (this->state->currentScene) {
        case SCENE_NONE: {
            ClearBackground(BLACK);
        } break;
        case SCENE_CRASH: {
            ClearBackground(BLACK);
            const char* titleText = "Something went horribly wrong";
            const int titleFontSize = 36;
            DrawText(
                titleText,
                GetScreenWidth() / 2 - MeasureText(titleText, titleFontSize) / 2,
                GetScreenHeight() / 2 - titleFontSize / 2,
                titleFontSize, WHITE
            );
            
            const char* subtitleText = "Press SPACE to return to the main menu";
            const int subtitleFontSize = 20;
            DrawText(
                subtitleText,
                GetScreenWidth() / 2 - MeasureText(subtitleText, subtitleFontSize) / 2,
                GetScreenHeight() / 2 + titleFontSize / 2 + subtitleFontSize,
                subtitleFontSize, WHITE
            );
        } break;
        case SCENE_LOAD_ASSETS: {
            sceneloadassetsDraw(this->sceneloadassets);
        } break;
        case SCENE_LEVEL: {
            scenelevelDraw(this->scenelevel);

            #ifdef DEBUG
                int y = 34 + 24;
                const char* labelTimeText = TextFormat("Time: %llf", this->scenelevel->player.timeAlive);
                DrawText(labelTimeText, 10, y, 24, WHITE);
                y += 24;
                const char* labelXText = TextFormat("X: %f", this->scenelevel->player.position.x);
                DrawText(labelXText, 10, y, 24, WHITE);
                y += 24;
                const char* labelYText = TextFormat("Y: %f", this->scenelevel->player.position.y);
                DrawText(labelYText, 10, y, 24, WHITE);
                y += 24;
                const char* labelVelXText = TextFormat("velX: %f", this->scenelevel->player.velocity.x);
                DrawText(labelVelXText, 10, y, 24, WHITE);
                y += 24;
                const char* labelVelYText = TextFormat("velY: %f", this->scenelevel->player.velocity.y);
                DrawText(labelVelYText, 10, y, 24, WHITE);
                y += 24;
                const char* labelOnGroundText = TextFormat("Grounded: %s", this->scenelevel->player.isOnGround ? "true" : "false");
                DrawText(labelOnGroundText, 10, y, 24, WHITE);
            #endif
        } break;
        case SCENE_LVLED: {
            scenelvledDraw(this->scenelvled);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene draw loop is not implemented");
        } break;
    }
}
