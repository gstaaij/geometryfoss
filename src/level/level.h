#pragma once
#include "nob.h"
#include "cJSON/cJSON.h"
#include "object.h"
#include "level/levelsettings.h"

// Serialize a level to cJSON
cJSON* levelSerialize(const LevelSettings levelSettings, const Object* objects);
// Serialize a level to a JSON string builder using cJSON
Nob_String_Builder levelSerializeStringBuilder(const LevelSettings levelSettings, const Object* objects);
// Deserialize a level from a JSON string builder
bool levelDeserialize(LevelSettings* levelSettings, Object** objects, const Nob_String_Builder lvlJsonString);
