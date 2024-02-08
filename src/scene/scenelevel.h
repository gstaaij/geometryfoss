#pragma once
#include "nob.h"
#include "scene/sceneswitcher.h"
#include "object.h"
#include "player.h"
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
void scenelevelDestroy(SceneLevel* scenelevel);

void scenelevelUpdate(SceneLevel* scenelevel, SceneState* sceneState, double deltaTime);

void scenelevelUpdateUI(SceneLevel* scenelevel, SceneState* sceneState);

void scenelevelDraw(SceneLevel* scenelevel);
