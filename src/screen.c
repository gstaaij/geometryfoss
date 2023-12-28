#include "screen.h"
#include <math.h>

// The minimum GD coords screen size according to GD
// Measured using the Camera Guide trigger in GD 2.2
#define MIN_COORD_SCREEN_WIDTH 480
#define MIN_COORD_SCREEN_HEIGHT 320

Coord getScreenSizeAsCoord(int screenWidth, int screenHeight) {
    // First try making the height the same as the minimum height...
    double height = MIN_COORD_SCREEN_HEIGHT;
    // ... and calculate a width based on that
    double width = (double)screenWidth / (double)screenHeight * MIN_COORD_SCREEN_HEIGHT;

    // If that doesn't work...
    if (width < MIN_COORD_SCREEN_WIDTH) {
        // ... make the width the same as the minimum width...
        width = MIN_COORD_SCREEN_WIDTH;
        // ... and calculate the height based on that width
        height = (double)screenHeight / (double)screenWidth * MIN_COORD_SCREEN_WIDTH;
    }

    // Construct a Coord out of the width and height variables and return it
    Coord screenSize = {
        .x = width,
        .y = height,
    };
    return screenSize;
}

ScreenCoord getScreenCoord(const Coord coord, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    // Translate the coordinates so (0, 0) is the top left of the screen and y increases when going downward
    double xTranslated = coord.x - cameraCoord.x + (screenSizeAsCoord.x / 2);
    double yTranslated = -(coord.y - cameraCoord.y) + (screenSizeAsCoord.y / 2);
    // Map the GD coordinates to screen coordinates
    long xScreen = roundl(xTranslated / screenSizeAsCoord.x * screenSize.x);
    long yScreen = roundl(yTranslated / screenSizeAsCoord.y * screenSize.y);

    // Put the coordinates in a ScreenCoord and return it
    ScreenCoord screenCoord = {
        .x = xScreen,
        .y = yScreen,
    };
    return screenCoord;
}

long convertToScreen(double size, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    // Multiply the size by the ratio between the screen coordinates and the GD coordinates
    return roundl((double)screenSize.x / screenSizeAsCoord.x * size);
}
