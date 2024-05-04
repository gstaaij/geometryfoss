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
void scenemanagerDestroy(SceneManager* this);

void scenemanagerLoad(SceneManager* this, const SceneEnum scene);
void scenemanagerUnload(SceneManager* this);

void scenemanagerUpdate(SceneManager* this, const double deltaTime);

void scenemanagerUpdateUI(SceneManager* this);

void scenemanagerDraw(SceneManager* this);
