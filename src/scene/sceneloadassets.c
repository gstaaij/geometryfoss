#include "sceneloadassets.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

SceneLoadAssets* sceneloadassetsCreate() {
    SCENE_CREATE(SceneLoadAssets);

    this->state = SLASTATE_AWAITING_DRAG_DROP;

    return this;
}

void sceneloadassetsDestroy(SceneLoadAssets* this) {
    free(this);
}

void sceneloadassetsUpdate(SceneLoadAssets* this, SceneState* sceneState, double deltaTime) {
    (void) this;
    (void) sceneState;
    (void) deltaTime;
}

void sceneloadassetsUpdateUI(SceneLoadAssets* this, SceneState* sceneState) {
    (void) this;
    (void) sceneState;
}

void sceneloadassetsDraw(SceneLoadAssets* this) {
    ClearBackground(ColorFromHSV(0, 0.0, 0.25));

    switch (this->state) {
        case SLASTATE_AWAITING_DRAG_DROP: {
            DrawText("Drag GeometryDash.exe to this window or click here to locate it", GetScreenWidth() / 2, GetScreenHeight() / 2, 24, WHITE);
        } break;
        case SLASTATE_EXTRACTING_ASSETS: {
            assert(false && "Not implemented");
        } break;
    }
}
