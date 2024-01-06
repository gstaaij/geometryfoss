#pragma once
#include "coord.h"
#include "player.h"

// Update the camera so it follows the player
void cameraUpdate(GDFCamera* camera, const Player player, const double deltaTime);

// Convert the size of the screen to GD coordinates
void cameraConvertScreenSize(GDFCamera* camera);

// Recalculate the size of the screen in pixels and GD coordinates
void cameraRecalculateScreenSize(GDFCamera* camera);

// Convert a position in GD coordinates to a position screen coordinates
ScreenCoord getScreenCoord(const Coord coord, const GDFCamera camera);

// Convert a width or height or length in GD coordinates to screen coordinates
long convertToScreen(const double size, const GDFCamera camera);