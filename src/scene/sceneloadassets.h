#pragma once
#include <stdbool.h>

#include "scene/sceneswitcher.h"
#include "raylib.h"
#include "camera.h"

typedef enum {
    SLASTATE_AWAITING_DRAG_DROP,
    SLASTATE_EXTRACTING_ASSETS,
    SLASTATE_COPY_ERROR,
} SLAState;

typedef struct {
    SLAState state;
    bool alreadyHasAssets;
    size_t currentAssetIndex;
    char* GDDirectory;
    const char* errorMessage;
    Font font;
    GDFCamera uiCamera;
} SceneLoadAssets;

SceneLoadAssets* sceneloadassetsCreate();
void sceneloadassetsDestroy(SceneLoadAssets* this);

void sceneloadassetsUpdate(SceneLoadAssets* this, SceneState* sceneState, double deltaTime);

void sceneloadassetsUpdateUI(SceneLoadAssets* this, SceneState* sceneState);

void sceneloadassetsDraw(SceneLoadAssets* this);
