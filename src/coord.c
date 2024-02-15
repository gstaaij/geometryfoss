#include "raylib.h"
#include "coord.h"
#include "serialize.h"

cJSON* coordSerialize(const Coord coord) {

    cJSON* coordJson = cJSON_CreateObject();

    cJSON* result = coordJson;

    if (cJSON_AddNumberToObject(coordJson, "x", coord.x) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize coord x");
        nob_return_defer(NULL);
    }

    if (cJSON_AddNumberToObject(coordJson, "y", coord.y) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize coord y");
        nob_return_defer(NULL);
    }

defer:
    if (result == NULL)
        cJSON_Delete(coordJson);
    return result;
}

bool coordDeserialize(Coord* coord, const cJSON* coordJson) {
    const cJSON* xJson = cJSON_GetObjectItemCaseSensitive(coordJson, "x");
    if (cJSON_IsNumber(xJson)) {
        coord->x = xJson->valueint;
    }

    const cJSON* yJson = cJSON_GetObjectItemCaseSensitive(coordJson, "y");
    if (cJSON_IsNumber(yJson)) {
        coord->y = yJson->valueint;
    }

    return true;
}
