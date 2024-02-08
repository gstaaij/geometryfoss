#include "scenemanager.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "raylib.h"

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
        } break;
        case SCENE_LVLED: {
            scenelvledDraw(scenemanager->scenelvled);
        } break;
        default: {
            assert(false && "NOT IMPLEMENTED: a scene draw loop is not implemented");
        } break;
    }
}
