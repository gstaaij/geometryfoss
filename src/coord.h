#pragma once

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
