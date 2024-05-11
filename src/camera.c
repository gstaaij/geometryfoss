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
    // Make sure the zoom isn't zero
    if (camera->actualZoom == 0)
        cameraSetZoomLevel(camera, 1.0);

    // First try making the height the same as the minimum height...
    double height = MIN_COORD_SCREEN_HEIGHT;
    // ... and calculate a width based on that
    double width = camera->screenSize.x / camera->screenSize.y * MIN_COORD_SCREEN_HEIGHT;

    // If that doesn't work...
    if (width < MIN_COORD_SCREEN_WIDTH) {
        // ... make the width the same as the minimum width...
        width = MIN_COORD_SCREEN_WIDTH;
        // ... and calculate the height based on that width
        height = camera->screenSize.y / camera->screenSize.x * MIN_COORD_SCREEN_WIDTH;
    }

    // Construct a Coord out of the width and height variables and assign it to camera.screenSizeAsCoord
    camera->screenSizeAsCoord = (Coord){
        .x = width * camera->actualZoom,
        .y = height * camera->actualZoom,
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

void cameraSetZoomLevel(GDFCamera* camera, const double zoomLevel) {
    camera->zoomLevel = zoomLevel;
    // Limit the zoom level
    if (camera->zoomLevel > 4)
        camera->zoomLevel = 4;
    if (camera->zoomLevel < -3)
        camera->zoomLevel = -3;
    
    camera->actualZoom = pow(2, - camera->zoomLevel + 1);
}

ScreenCoord getScreenCoord(const Coord coord, const GDFCamera camera) {
    // Translate the coordinates so (0, 0) is the top left of the screen and y increases when going downward
    double xTranslated = coord.x - camera.position.x + (camera.screenSizeAsCoord.x / 2);
    double yTranslated = -(coord.y - camera.position.y) + (camera.screenSizeAsCoord.y / 2);
    // Map the GD coordinates to screen coordinates
    double xScreen = xTranslated / camera.screenSizeAsCoord.x * camera.screenSize.x;
    double yScreen = yTranslated / camera.screenSizeAsCoord.y * camera.screenSize.y;

    // Put the coordinates in a ScreenCoord and return it
    ScreenCoord screenCoord = {
        .x = xScreen,
        .y = yScreen,
    };
    return screenCoord;
}

Coord getGDCoord(const ScreenCoord screenCoord, const GDFCamera camera) {
    // The reverse of what getScreenCoord does
    
    double xTemp = screenCoord.x / camera.screenSize.x * camera.screenSizeAsCoord.x;
    double yTemp = screenCoord.y / camera.screenSize.y * camera.screenSizeAsCoord.y;

    double xGD = xTemp - (camera.screenSizeAsCoord.x / 2) + camera.position.x;
    double yGD = -(yTemp - (camera.screenSizeAsCoord.y / 2)) + camera.position.y;

    // Put the coordinates in a Coord and return it
    Coord coord = {
        .x = xGD,
        .y = yGD,
    };
    return coord;
}

double convertToScreen(const double size, const GDFCamera camera) {
    // Multiply the size by the ratio between the screen coordinates and the GD coordinates
    return camera.screenSize.x / camera.screenSizeAsCoord.x * size;
}

double convertToGD(const double size, const GDFCamera camera) {
    // Multiply the size by the ratio between the GD coordinates and the screen coordinates
    return camera.screenSizeAsCoord.x / camera.screenSize.x * size;
}
