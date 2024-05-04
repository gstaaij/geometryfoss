#pragma once
#include <stdlib.h>
#include "scene/sceneswitcher.h"
#include "object.h"
#include "camera.h"
#include "level/levelsettings.h"
// #include "hashmaps.h"

typedef enum {
    EDITOR_UI_MODE_BUILD,
    EDITOR_UI_MODE_EDIT,
    EDITOR_UI_MODE_DELETE,
} EditorUIMode;

typedef struct {
    Object* objects;
    GDFCamera camera;
    GDFCamera uiCamera;
    EditorUIMode uiMode;

    LevelSettings levelSettings;

    // Build Mode

    int blockBuildId;

    // General state management stuff

    bool isPaused;
    bool clickedButton;
} SceneLevelEditor;

SceneLevelEditor* scenelvledCreate();
void scenelvledDestroy(SceneLevelEditor* this);

void scenelvledUpdate(SceneLevelEditor* this, SceneState* sceneState, double deltaTime);

void scenelvledUpdateUI(SceneLevelEditor* this, SceneState* sceneState);

void scenelvledDraw(SceneLevelEditor* this);
