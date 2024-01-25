#include "level.h"
#include "cJSON/cJSON.h"
#include "stb_ds.h"

#define _levelSerialize(scene, lvlJson) do { \
    if (cJSON_AddNumberToObject(lvlJson, "backgroundColor", ColorToInt(scene->backgroundColor)) == NULL) {          \
        nob_log(NOB_ERROR, "Couldn't serialize level background color");                                            \
        goto defer;                                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    if (cJSON_AddNumberToObject(lvlJson, "groundColor", ColorToInt(scene->groundColor)) == NULL) {                  \
        nob_log(NOB_ERROR, "Couldn't serialize level ground color");                                                \
        goto defer;                                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    cJSON* objectsJson = cJSON_AddArrayToObject(lvlJson, "objects");                                                \
    if (objectsJson == NULL) {                                                                                      \
        nob_log(NOB_ERROR, "Couldn't add level object array");                                                      \
        goto defer;                                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    const size_t len = arrlenu(scene->objects);                                                                     \
    for (size_t i = 0; i < len; ++i) {                                                                              \
        cJSON* objectJson = objectSerialize(scene->objects[i]);                                                     \
                                                                                                                    \
        if (objectJson == NULL) {                                                                                   \
            nob_log(NOB_ERROR, "Couldn't serialize level object %d", i);                                            \
            goto defer;                                                                                             \
        }                                                                                                           \
                                                                                                                    \
        cJSON_AddItemToArray(objectsJson, objectJson);                                                              \
    }                                                                                                               \
} while (0)

#define _levelSerializeString(scene) do {                                                                           \
    Nob_String_Builder stringbJson = {0};                                                                           \
                                                                                                                    \
    cJSON* lvlJson = cJSON_CreateObject();                                                                          \
                                                                                                                    \
    _levelSerialize(scene, lvlJson);                                                                                \
                                                                                                                    \
    /* TODO: once this all works perfectly, replace cJSON_Print with cJSON_PrintUnformatted to reduce file size */  \
    char* string = cJSON_Print(lvlJson);                                                                            \
    if (string == NULL) {                                                                                           \
        nob_log(NOB_ERROR, "Couldn't print level JSON to string");                                                  \
        goto defer;                                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    nob_sb_append_cstr(&stringbJson, string);                                                                       \
                                                                                                                    \
    free(string);                                                                                                   \
                                                                                                                    \
defer:                                                                                                              \
    cJSON_Delete(lvlJson);                                                                                          \
    return stringbJson;                                                                                             \
} while (0)

Nob_String_Builder scenelvledSerialize(const SceneLevelEditor* scenelvled) {
    _levelSerializeString(scenelvled);
}

Nob_String_Builder scenelevelSerialize(const SceneLevel* scenelevel) {
    _levelSerializeString(scenelevel);
}

#define _levelDeserialize(scene, lvlJsonString) do {                                                \
    bool result = true;                                                                             \
                                                                                                    \
    cJSON* lvlJson = cJSON_ParseWithLength(lvlJsonString.items, lvlJsonString.count);               \
    if (lvlJson == NULL) {                                                                          \
        const char* errorPtr = cJSON_GetErrorPtr();                                                 \
        nob_log(NOB_ERROR, "Failed parsing level JSON", errorPtr);                                  \
        if (errorPtr != NULL) {                                                                     \
            nob_log(NOB_ERROR, "cJSON error pointer: %s", errorPtr);                                \
        }                                                                                           \
        nob_return_defer(false);                                                                    \
    }                                                                                               \
                                                                                                    \
    const cJSON* backgroundColor = cJSON_GetObjectItemCaseSensitive(lvlJson, "backgroundColor");    \
    if (cJSON_IsNumber(backgroundColor)) {                                                          \
        scene->backgroundColor = GetColor((unsigned int) backgroundColor->valueint);                \
    }                                                                                               \
                                                                                                    \
    const cJSON* groundColor = cJSON_GetObjectItemCaseSensitive(lvlJson, "groundColor");            \
    if (cJSON_IsNumber(groundColor)) {                                                              \
        scene->groundColor = GetColor((unsigned int) groundColor->valueint);                        \
    }                                                                                               \
                                                                                                    \
    const cJSON* objects = cJSON_GetObjectItemCaseSensitive(lvlJson, "objects");                    \
    const cJSON* object;                                                                            \
    arrfree(scene->objects);                                                                        \
    if (cJSON_IsArray(objects)) {                                                                   \
        cJSON_ArrayForEach(object, objects) {                                                       \
            Object newObject = {0};                                                                 \
            if (!objectDeserialize(&newObject, object))                                             \
                nob_log(NOB_WARNING, "Failed to parse object, skipping...");                        \
            arrput(scene->objects, newObject);                                                      \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    nob_log(NOB_INFO, "Loaded %d objects", arrlen(scene->objects));                                 \
                                                                                                    \
defer:                                                                                              \
    cJSON_Delete(lvlJson);                                                                          \
    return result;                                                                                  \
} while (0)

bool scenelvledDeserialize(SceneLevelEditor* scenelvled, const Nob_String_Builder lvlJsonString) {
    _levelDeserialize(scenelvled, lvlJsonString);
}

bool scenelevelDeserialize(SceneLevel* scenelevel, const Nob_String_Builder lvlJsonString) {
    _levelDeserialize(scenelevel, lvlJsonString);
}
