#include "level.h"
#include "stb_ds.h"


cJSON* levelSerialize(const LevelSettings levelSettings, const Object* objects) {
    cJSON* lvlJson = cJSON_CreateObject();

    cJSON* result = lvlJson;

    cJSON* lvlsettingsJson = levelsettingsSerialize(levelSettings);
    if (lvlsettingsJson == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize level settings");
        nob_return_defer(NULL);
    }
    cJSON_AddItemToObject(lvlJson, "settings", lvlsettingsJson);

    cJSON* objectsJson = cJSON_AddArrayToObject(lvlJson, "objects");
    if (objectsJson == NULL) {
        TraceLog(LOG_ERROR, "Couldn't add level object array");
        nob_return_defer(NULL);
    }

    const size_t len = arrlenu(objects);
    for (size_t i = 0; i < len; ++i) {
        cJSON* objectJson = objectSerialize(objects[i]);

        if (objectJson == NULL) {
            TraceLog(LOG_ERROR, "Couldn't serialize level object %d", i);
            nob_return_defer(NULL);
        }

        cJSON_AddItemToArray(objectsJson, objectJson);
    }

defer:
    if (result == NULL)
        cJSON_Delete(lvlJson);
    return result;
}

Nob_String_Builder levelSerializeStringBuilder(const LevelSettings levelSettings, const Object* objects) {
    Nob_String_Builder stringbJson = {0};

    cJSON* lvlJson = levelSerialize(levelSettings, objects);
    if (lvlJson == NULL)
        goto defer;
    
    /* TODO: once this all works perfectly, replace cJSON_Print with cJSON_PrintUnformatted to reduce file size */
    char* string = cJSON_Print(lvlJson);
    if (string == NULL) {
        TraceLog(LOG_ERROR, "Couldn't print level JSON to string");
        goto defer;
    }

    nob_sb_append_cstr(&stringbJson, string);

    free(string);

defer:
    cJSON_Delete(lvlJson);
    return stringbJson;
}

bool levelDeserialize(LevelSettings* levelSettings, Object** objects, const Nob_String_Builder lvlJsonString) {
    bool result = true;

    cJSON* lvlJson = cJSON_ParseWithLength(lvlJsonString.items, lvlJsonString.count);
    if (lvlJson == NULL) {
        const char* errorPtr = cJSON_GetErrorPtr();
        TraceLog(LOG_ERROR, "Failed parsing level JSON", errorPtr);
        if (errorPtr != NULL) {
            TraceLog(LOG_ERROR, "cJSON error pointer: %s", errorPtr);
        }
        nob_return_defer(false);
    }

    const cJSON* lvlsettingsJson = cJSON_GetObjectItemCaseSensitive(lvlJson, "settings");
    if (!levelsettingsDeserialize(levelSettings, lvlsettingsJson)) {
        TraceLog(LOG_ERROR, "Couldn't load level settings, aborting...");
        nob_return_defer(false);
    }

    const cJSON* objectsJson = cJSON_GetObjectItemCaseSensitive(lvlJson, "objects");
    const cJSON* objectJson;
    arrfree(*objects);
    if (cJSON_IsArray(objectsJson)) {
        cJSON_ArrayForEach(objectJson, objectsJson) {
            Object newObject = {0};
            if (!objectDeserialize(&newObject, objectJson))
                TraceLog(LOG_WARNING, "Failed to parse object, skipping...");
            arrput(*objects, newObject);
        }
    }

    TraceLog(LOG_INFO, "Loaded %d objects", arrlen(*objects));

defer:
    cJSON_Delete(lvlJson);
    return result;
}

bool levelSaveToFile(const char* relativeFilePath, const LevelSettings levelSettings, const Object* objects) {
    Nob_String_Builder lvlJson = levelSerializeStringBuilder(levelSettings, objects);
    const char* levelFilePath = TextFormat("%s/%s", GetApplicationDirectory(), relativeFilePath);
    if (lvlJson.count != 0) {
        return nob_write_entire_file(levelFilePath, lvlJson.items, lvlJson.count);
    }
    TraceLog(LOG_ERROR, "Couldn't save level \"%s\"", levelFilePath);
    return false;
}

bool levelLoadFromFile(const char* relativeFilePath, LevelSettings* levelSettings, Object** objects) {
    Nob_String_Builder lvlJson = {0};

    const char* levelFilePath = TextFormat("%s/%s", GetApplicationDirectory(), relativeFilePath);
    if (nob_read_entire_file(levelFilePath, &lvlJson)) {
        return levelDeserialize(levelSettings, objects, lvlJson);
    }
    TraceLog(LOG_ERROR, "Couldn't load level \"%s\"", levelFilePath);
    return false;
}
