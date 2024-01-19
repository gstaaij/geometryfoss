#include "coord.h"
#include "serialize.h"

Nob_String_Builder coordSerialize(const Coord coord, const int tabSize) {
    Nob_String_Builder coordJson = {0};

    nob_sb_append_cstr(&coordJson, "{\n");

    // X
    serializeTAB(&coordJson, tabSize + 1);
    serializePROPERTY(&coordJson, "x");
    nob_sb_append_cstr(&coordJson, nob_temp_sprintf("%lf", coord.x));
    nob_sb_append_cstr(&coordJson, ",\n");

    // Y
    serializeTAB(&coordJson, tabSize + 1);
    serializePROPERTY(&coordJson, "y");
    nob_sb_append_cstr(&coordJson, nob_temp_sprintf("%lf", coord.y));
    nob_da_append(&coordJson, '\n');

    serializeTAB(&coordJson, tabSize);
    nob_da_append(&coordJson, '}');

    return coordJson;
}
