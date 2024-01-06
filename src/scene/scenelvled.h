#pragma once
#include "dynamicarrays.h"
#include "camera.h"

typedef struct {
    DAObjects objects;
    GDFCamera camera;
} SceneLevelEditor;

SceneLevelEditor* scenelvledCreate();
void scenelvledDestroy(SceneLevelEditor* scenelvled);

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime);

void scenelvledDraw(SceneLevelEditor* scenelvled);
