#pragma once
#include "camera.h"
#include "scene/scenelevel.h"
#include "scene/scenelvled.h"

typedef enum {
    SCENE_NONE = 0,
    SCENE_LEVEL,
    SCENE_LVLED,
} SceneEnum; // I'm very good at naming things

typedef struct {
    SceneEnum currentScene;
    SceneLevelEditor* scenelvled;
    SceneLevel* scenelevel;
} SceneManager;

SceneManager* scenemanagerCreate();
void scenemanagerDestroy(SceneManager* scenemanager);

void scenemanagerLoad(SceneManager* scenemanager, const SceneEnum scene);
void scenemanagerUnload(SceneManager* scenemanager);

void scenemanagerUpdate(SceneManager* scenemanager, const double deltaTime);

void scenemanagerDraw(SceneManager* scenemanager);
