#pragma once
#include "nob.h"
#include "lib/cJSON/cJSON.h"
#include "object.h"
#include "level/levelsettings.h"

// Serialize a level to cJSON
cJSON* levelSerialize(const LevelSettings levelSettings, const Object* objects);
// Serialize a level to a JSON string builder using cJSON
Nob_String_Builder levelSerializeStringBuilder(const LevelSettings levelSettings, const Object* objects);
// Deserialize a level from a JSON string builder
bool levelDeserialize(LevelSettings* levelSettings, Object** objects, const Nob_String_Builder lvlJsonString);

// Save a level to a JSON file
bool levelSaveToFile(const char* relativeFilePath, const LevelSettings levelSettings, const Object* objects);
// Load a level from a JSON file
bool levelLoadFromFile(const char* relativeFilePath, LevelSettings* levelSettings, Object** objects);
