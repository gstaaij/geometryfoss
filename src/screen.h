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

// Convert the size of the screen to GD coordinates
Coord getScreenSizeAsCoord(int screenWidth, int screenHeight);

// Convert a position in GD coordinates to a position screen coordinates
ScreenCoord getScreenCoord(const Coord coord, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize);

// Convert a width or height or length in GD coordinates to screen coordinates
long convertToScreen(double size, const Coord screenSizeAsCoord, const ScreenCoord screenSize);
