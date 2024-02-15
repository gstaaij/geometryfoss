#include "levelsettings.h"
#include "nob.h"

cJSON* levelsettingsSerialize(const LevelSettings levelSettings) {
    cJSON* lvlsettingsJson = cJSON_CreateObject();

    cJSON* result = lvlsettingsJson;

    if (cJSON_AddNumberToObject(lvlsettingsJson, "backgroundColor", ColorToInt(levelSettings.backgroundColor)) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize level background color");
        nob_return_defer(NULL);
    }

    if (cJSON_AddNumberToObject(lvlsettingsJson, "groundColor", ColorToInt(levelSettings.groundColor)) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize level ground color");
        nob_return_defer(NULL);
    }

defer:
    if (result == NULL)
        cJSON_Delete(lvlsettingsJson);
    return result;
}

bool levelsettingsDeserialize(LevelSettings* levelSettings, const cJSON* lvlsettingsJson) {

    const cJSON* backgroundColorJson = cJSON_GetObjectItemCaseSensitive(lvlsettingsJson, "backgroundColor");
    if (cJSON_IsNumber(backgroundColorJson)) {
        levelSettings->backgroundColor = GetColor((unsigned int) backgroundColorJson->valueint);
    }

    const cJSON* groundColorJson = cJSON_GetObjectItemCaseSensitive(lvlsettingsJson, "groundColor");
    if (cJSON_IsNumber(groundColorJson)) {
        levelSettings->groundColor = GetColor((unsigned int) groundColorJson->valueint);
    }

    return true;
}
