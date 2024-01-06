#include "camera.h"
#include "ground.h"
#include <math.h>

// The minimum GD coords screen size according to GD
// Measured using the Camera Guide trigger in GD 2.2
#define MIN_COORD_SCREEN_WIDTH 480
#define MIN_COORD_SCREEN_HEIGHT 320

// The distance between the camera and the player, also measured with the Camera Guide trigger
#define CAMERA_PLAYER_OFFSET_X 75

void cameraUpdate(GDFCamera* camera, const Player player, const double deltaTime) {
    // Always follow the player on the x axis at a set offset
    camera->position.x = player.position.x + CAMERA_PLAYER_OFFSET_X;

    /// TODO: follow player on the y position smoothly
    camera->position.y = player.position.y;
    (void) deltaTime;

    // Make sure the bottom of the camera doesn't go below y=0
    if (camera->position.y - (camera->screenSizeAsCoord.y / 2) < 0) {
        camera->position.y = camera->screenSizeAsCoord.y / 2;
    }
}

void cameraConvertScreenSize(GDFCamera* camera) {
    // First try making the height the same as the minimum height...
    double height = MIN_COORD_SCREEN_HEIGHT;
    // ... and calculate a width based on that
    double width = (double)camera->screenSize.x / (double)camera->screenSize.y * MIN_COORD_SCREEN_HEIGHT;

    // If that doesn't work...
    if (width < MIN_COORD_SCREEN_WIDTH) {
        // ... make the width the same as the minimum width...
        width = MIN_COORD_SCREEN_WIDTH;
        // ... and calculate the height based on that width
        height = (double)camera->screenSize.y / (double)camera->screenSize.x * MIN_COORD_SCREEN_WIDTH;
    }

    // Construct a Coord out of the width and height variables and assign it to camera.screenSizeAsCoord
    camera->screenSizeAsCoord = (Coord){
        .x = width,
        .y = height,
    };
}

void cameraRecalculateScreenSize(GDFCamera* camera) {
    // Set the screen size of the camera
    camera->screenSize = (ScreenCoord){
        .x = GetScreenWidth(),
        .y = GetScreenHeight(),
    };
    // Convert the screen size to GD coordinates
    cameraConvertScreenSize(camera);
}

ScreenCoord getScreenCoord(const Coord coord, const GDFCamera camera) {
    // Translate the coordinates so (0, 0) is the top left of the screen and y increases when going downward
    double xTranslated = coord.x - camera.position.x + (camera.screenSizeAsCoord.x / 2);
    double yTranslated = -(coord.y - camera.position.y) + (camera.screenSizeAsCoord.y / 2);
    // Map the GD coordinates to screen coordinates
    long xScreen = roundl(xTranslated / camera.screenSizeAsCoord.x * camera.screenSize.x);
    long yScreen = roundl(yTranslated / camera.screenSizeAsCoord.y * camera.screenSize.y);

    // Put the coordinates in a ScreenCoord and return it
    ScreenCoord screenCoord = {
        .x = xScreen,
        .y = yScreen,
    };
    return screenCoord;
}

long convertToScreen(const double size, const GDFCamera camera) {
    // Multiply the size by the ratio between the screen coordinates and the GD coordinates
    return roundl((double)camera.screenSize.x / camera.screenSizeAsCoord.x * size);
}
