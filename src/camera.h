#pragma once
#include "coord.h"
#include "player/player.h"

// Update the camera so it follows the player
void cameraUpdate(GDFCamera* camera, const Player player, const double deltaTime);

// Convert the size of the screen to GD coordinates
void cameraConvertScreenSize(GDFCamera* camera);

// Recalculate the size of the screen in pixels and GD coordinates
void cameraRecalculateScreenSize(GDFCamera* camera);

// Set the zoom level of a camera
void cameraSetZoomLevel(GDFCamera* camera, const double zoomLevel);

// Convert a position in GD coordinates to a position screen coordinates
ScreenCoord getScreenCoord(const Coord coord, const GDFCamera camera);
// Convert a position in screen coordinates to a position GD coordinates
Coord getGDCoord(const ScreenCoord coord, const GDFCamera camera);

// Convert a width or height or length in GD coordinates to screen coordinates
double convertToScreen(const double size, const GDFCamera camera);
// Convert a width or height or length in screen coordinates to GD coordinates
double convertToGD(const double size, const GDFCamera camera);
