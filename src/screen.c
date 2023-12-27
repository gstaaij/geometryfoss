#include "screen.h"
#include <math.h>

#define MIN_COORD_SCREEN_WIDTH 480
#define MIN_COORD_SCREEN_HEIGHT 320

Coord getScreenSizeAsCoord(int screenWidth, int screenHeight) {
    double height = MIN_COORD_SCREEN_HEIGHT;
    double width = (double)screenWidth / (double)screenHeight * MIN_COORD_SCREEN_HEIGHT;
    if (width < MIN_COORD_SCREEN_WIDTH) {
        width = MIN_COORD_SCREEN_WIDTH;
        height = (double)screenHeight / (double)screenWidth * MIN_COORD_SCREEN_WIDTH;
    }

    Coord screenSize = {
        .x = width,
        .y = height,
    };

    return screenSize;
}

ScreenCoord getScreenCoord(const Coord coord, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    double xTranslated = coord.x - cameraCoord.x + (screenSizeAsCoord.x / 2);
    double yTranslated = -(coord.y - cameraCoord.y) + (screenSizeAsCoord.y / 2);
    long xScreen = roundl(xTranslated / screenSizeAsCoord.x * screenSize.x);
    long yScreen = roundl(yTranslated / screenSizeAsCoord.y * screenSize.y);
    ScreenCoord screenCoord = {
        .x = xScreen,
        .y = yScreen,
    };
    return screenCoord;
}

long convertToScreen(double size, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    return (double)screenSize.x / screenSizeAsCoord.x * size;
}
