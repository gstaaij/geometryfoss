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
