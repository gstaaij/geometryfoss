#pragma once
#include "object.h"
#include "camera.h"

typedef struct {
    Object* objects;
    GDFCamera camera;
} SceneLevelEditor;

SceneLevelEditor* scenelvledCreate();
void scenelvledDestroy(SceneLevelEditor* scenelvled);

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime);

void scenelvledDraw(SceneLevelEditor* scenelvled);
