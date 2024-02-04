#pragma once
#include "nob.h"
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

void scenelevelUpdate(SceneLevel* scenelevel, double deltaTime);

void scenelevelUpdateUI(SceneLevel* scenelevel);

void scenelevelDraw(SceneLevel* scenelevel);
