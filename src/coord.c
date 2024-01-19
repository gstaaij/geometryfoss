#include "raylib.h"
#include "coord.h"
#include "serialize.h"

Nob_String_Builder coordSerialize(const Coord coord, const int tabSize) {
    Nob_String_Builder coordJson = {0};

    // Begin block
    nob_sb_append_cstr(&coordJson, "{\n");

    // x
    serializeTAB(&coordJson, tabSize + 1);
    serializePROPERTY(&coordJson, "x");
    nob_sb_append_cstr(&coordJson, TextFormat("%lf", coord.x));
    nob_sb_append_cstr(&coordJson, ",\n");

    // y
    serializeTAB(&coordJson, tabSize + 1);
    serializePROPERTY(&coordJson, "y");
    nob_sb_append_cstr(&coordJson, TextFormat("%lf", coord.y));
    nob_da_append(&coordJson, '\n');

    // End block
    serializeTAB(&coordJson, tabSize);
    nob_da_append(&coordJson, '}');

    return coordJson;
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
