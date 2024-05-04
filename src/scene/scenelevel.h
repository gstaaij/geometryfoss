#pragma once
#include "scene/sceneswitcher.h"
#include "object.h"
#include "player/player.h"
#include "camera.h"
#include "level/levelsettings.h"

typedef struct {
    Object* objects;
    Player player;
    GDFCamera camera;
    
    LevelSettings levelSettings;

    // Hacks (will be a better system at some point)
    bool frameStep;
} SceneLevel;

SceneLevel* scenelevelCreate();
void scenelevelDestroy(SceneLevel* this);

void scenelevelUpdate(SceneLevel* this, SceneState* sceneState, double deltaTime);

void scenelevelUpdateUI(SceneLevel* this, SceneState* sceneState);

void scenelevelDraw(SceneLevel* this);
