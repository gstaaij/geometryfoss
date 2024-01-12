#pragma once
#include "object.h"
#include "player.h"
#include "camera.h"

typedef struct {
    Object* objects;
    Player player;
    GDFCamera camera;
    Color backgroundColor;
    Color groundColor;

    // Hacks (will be a better system at some point)
    bool frameStep;
} SceneLevel;

SceneLevel* scenelevelCreate();
void scenelevelDestroy(SceneLevel* scenelevel);

void scenelevelUpdate(SceneLevel* scenelevel, double deltaTime);

void scenelevelUpdateUI(SceneLevel* scenelevel);

void scenelevelDraw(SceneLevel* scenelevel);
