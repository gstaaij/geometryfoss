#pragma once
#include "nob.h"
#include "scene/scenemanager.h"
#include "scene/scenelvled.h"
#include "scene/scenelevel.h"

// Serialize a SceneLevelEditor to a JSON string builder using cJSON
Nob_String_Builder scenelvledSerialize(const SceneLevelEditor* scenelvled);
// Serialize a SceneLevel to a JSON string builder using cJSON
Nob_String_Builder scenelevelSerialize(const SceneLevel* scenelevel);

// Deserialize a SceneLevelEditor from cJSON
bool scenelvledDeserialize(SceneLevelEditor* scenelvled, const Nob_String_Builder lvlJsonString);
// Deserialize a SceneLevel from cJSON
bool scenelevelDeserialize(SceneLevel* scenelevel, const Nob_String_Builder lvlJsonString);
