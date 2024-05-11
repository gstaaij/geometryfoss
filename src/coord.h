#pragma once
#include "nob.h"
#include "lib/cJSON/cJSON.h"

// Defines a position or size in screen coordinates
typedef struct {
    double x;
    double y;
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
    double zoomLevel;
    double actualZoom;
} GDFCamera;

// Serialize a Coord to cJSON
cJSON* coordSerialize(const Coord coord);
// Deserialize a Coord from cJSON
bool coordDeserialize(Coord* coord, const cJSON* coordJson);
