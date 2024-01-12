#pragma once
#include <stdlib.h>
#include "object.h"
#include "camera.h"
// #include "hashmaps.h"

typedef enum {
    EDITOR_UI_MODE_BUILD,
    EDITOR_UI_MODE_EDIT,
    EDITOR_UI_MODE_DELETE,
} EditorUIMode;

typedef struct {
    Object* objects;
    size_t* selectedObjects;
    GDFCamera camera;
    EditorUIMode uiMode;
    Color backgroundColor;
    Color groundColor;
} SceneLevelEditor;

SceneLevelEditor* scenelvledCreate();
void scenelvledDestroy(SceneLevelEditor* scenelvled);

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime);

void scenelvledUpdateUI(SceneLevelEditor* scenelvled);

void scenelvledDraw(SceneLevelEditor* scenelvled);
