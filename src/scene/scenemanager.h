#pragma once
#include "camera.h"
#include "scene/sceneswitcher.h"
#include "scene/scenelvled.h"
#include "scene/scenelevel.h"


typedef struct {
    SceneState* state;

    SceneLevelEditor* scenelvled;
    SceneLevel* scenelevel;
} SceneManager;

SceneManager* scenemanagerCreate();
void scenemanagerDestroy(SceneManager* scenemanager);

void scenemanagerLoad(SceneManager* scenemanager, const SceneEnum scene);
void scenemanagerUnload(SceneManager* scenemanager);

void scenemanagerUpdate(SceneManager* scenemanager, const double deltaTime);

void scenemanagerUpdateUI(SceneManager* scenemanager);

void scenemanagerDraw(SceneManager* scenemanager);
