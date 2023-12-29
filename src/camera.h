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

// Convert the size of the screen to GD coordinates
void cameraConvertScreenSize(GDFCamera* camera);

// Convert a position in GD coordinates to a position screen coordinates
ScreenCoord getScreenCoord(const Coord coord, const GDFCamera camera);

// Convert a width or height or length in GD coordinates to screen coordinates
long convertToScreen(double size, const GDFCamera camera);
