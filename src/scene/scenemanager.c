#include "scenemanager.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include "raylib.h"

SceneManager* scenemanagerCreate() {
    SceneManager* scenemanager = (SceneManager*) malloc(sizeof(SceneManager));
    assert(scenemanager != NULL && "You don't have enough RAM");
    memset(scenemanager, 0, sizeof(SceneManager));
    return scenemanager;
}
void scenemanagerDestroy(SceneManager* scenemanager) {
    free(scenemanager);
}

void scenemanagerLoad(SceneManager* scenemanager, const SceneEnum scene) {
    scenemanagerUnload(scenemanager);
    switch (scene) {
    case SCENE_NONE:
        break;
    case SCENE_LEVEL:
        scenemanager->scenelevel = scenelevelCreate();
        break;
    case SCENE_LVLED:
        scenemanager->scenelvled = scenelvledCreate();
        break;
    default:
        assert(false && "NOT IMPLEMENTED: a scene is not implemented");
    }
    scenemanager->currentScene = scene;
}

void scenemanagerUnload(SceneManager* scenemanager) {
    switch (scenemanager->currentScene) {
    case SCENE_LEVEL:
        scenelevelDestroy(scenemanager->scenelevel);
        break;
    case SCENE_LVLED:
        scenelvledDestroy(scenemanager->scenelvled);
        break;
    default:
        break;
    }
    scenemanager->currentScene = SCENE_NONE;
}

void scenemanagerUpdate(SceneManager* scenemanager, const double deltaTime) {
    switch (scenemanager->currentScene) {
    case SCENE_NONE:
        break;
    case SCENE_LEVEL:
        scenelevelUpdate(scenemanager->scenelevel, deltaTime);
        break;
    case SCENE_LVLED:
        scenelvledUpdate(scenemanager->scenelvled, deltaTime);
        break;
    default:
        assert(false && "NOT IMPLEMENTED: a scene update loop is not implemented");
    }
}

void scenemanagerDraw(SceneManager* scenemanager) {
    switch (scenemanager->currentScene) {
    case SCENE_NONE:
        ClearBackground(BLACK);
        break;
    case SCENE_LEVEL:
        scenelevelDraw(scenemanager->scenelevel);
        break;
    case SCENE_LVLED:
        scenelvledDraw(scenemanager->scenelvled);
        break;
    default:
        assert(false && "NOT IMPLEMENTED: a scene draw loop is not implemented");
    }
}
