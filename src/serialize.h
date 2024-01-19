#pragma once

// Insert 4 spaces per tabSize into a Nob_String_Builder
#define serializeTAB(sb, tabSize) for (int _ = 0; _ < (tabSize); ++_) nob_sb_append_cstr(sb, "    ")

// Insert a property ("property": )
#define serializePROPERTY(sb, property) nob_sb_append_cstr(sb, "\"" property "\": ")
