#pragma once
#include "raylib.h"
#include "lib/cJSON/cJSON.h"

typedef struct {
    Color backgroundColor;
    Color groundColor;
} LevelSettings;

// Serialize level settings to cJSON
cJSON* levelsettingsSerialize(const LevelSettings levelSettings);
// Deserialize level settings from cJSON
bool levelsettingsDeserialize(LevelSettings* levelSettings, const cJSON* lvlsettingsJson);
