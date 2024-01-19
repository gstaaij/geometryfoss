#pragma once
#include "nob.h"
#include "cJSON/cJSON.h"

// Defines a position or size in screen coordinates
typedef struct {
    long x;
    long y;
} ScreenCoord;

// Defines a position or size in GD coordinates
typedef struct {
    double x;
    double y;
} Coord;

// Defines a camera with a position, 
typedef struct {
    Coord position;
    Coord screenSizeAsCoord;
    ScreenCoord screenSize;
} GDFCamera;

// Serialize a Coord to a JSON string builder
Nob_String_Builder coordSerialize(const Coord coord, const int tabSize);
// Deserialize a Coord from cJSON
bool coordDeserialize(Coord* coord, const cJSON* coordJson);
