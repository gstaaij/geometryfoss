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
    GDFCamera camera;
    GDFCamera uiCamera;
    EditorUIMode uiMode;
    Color backgroundColor;
    Color groundColor;

    // Build Mode

    int blockBuildId;
} SceneLevelEditor;

SceneLevelEditor* scenelvledCreate();
void scenelvledDestroy(SceneLevelEditor* scenelvled);

// Serialize a LevelEditorScene to a JSON string builder using cJSON
Nob_String_Builder scenelvledSerialize(const SceneLevelEditor* scenelvled);
// Deserialize a LevelEditorScene from cJSON
bool scenelvledDeserialize(SceneLevelEditor* scenelvled, const Nob_String_Builder lvlJsonString);

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime);

void scenelvledUpdateUI(SceneLevelEditor* scenelvled);

void scenelvledDraw(SceneLevelEditor* scenelvled);
