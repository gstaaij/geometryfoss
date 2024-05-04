#pragma once
#include "scene/sceneswitcher.h"

typedef enum {
    SLASTATE_AWAITING_DRAG_DROP,
    SLASTATE_EXTRACTING_ASSETS,
} SLAState;

typedef struct {
    SLAState state;
} SceneLoadAssets;

SceneLoadAssets* sceneloadassetsCreate();
void sceneloadassetsDestroy(SceneLoadAssets* this);

void sceneloadassetsUpdate(SceneLoadAssets* this, SceneState* sceneState, double deltaTime);

void sceneloadassetsUpdateUI(SceneLoadAssets* this, SceneState* sceneState);

void sceneloadassetsDraw(SceneLoadAssets* this);
