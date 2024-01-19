#pragma once

#define serializeTAB(sb, n) for (int _ = 0; _ < (n); ++_) nob_sb_append_cstr(sb, "    ")

#define serializePROPERTY(sb, property) nob_sb_append_cstr(sb, "\"" property "\": ")
