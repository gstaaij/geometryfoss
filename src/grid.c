#include "grid.h"
#include "raylib.h"
#include <math.h>

void gridDraw(const GDFCamera camera) {
    // Calculate the bottom left of the screen in GD coordinates
    double left = camera.position.x - camera.screenSizeAsCoord.x / 2;
    double bottom = camera.position.y - camera.screenSizeAsCoord.y / 2;
    
    // The x position of the first vertical line
    double startX = floor(left / GRID_WIDTH) * GRID_WIDTH;
    // The y position of the first horizontal line
    double startY = floor(bottom / GRID_HEIGHT) * GRID_HEIGHT;

    // Calculate the top right of the screen in GD coordinates, to know where to stop drawing lines
    double endX = camera.position.x + camera.screenSizeAsCoord.x / 2;
    double endY = camera.position.y + camera.screenSizeAsCoord.y / 2;

    // Draw the vertical lines
    for (double x = startX; x < endX; x += GRID_WIDTH) {
        ScreenCoord scLine = getScreenCoord((Coord) { x, 0 }, camera);
        DrawLine(
            scLine.x,
            0,
            scLine.x,
            camera.screenSize.y,
            BLACK
        );
    }

    // Draw the horizontal lines
    for (double y = startY; y < endY; y += GRID_HEIGHT) {
        ScreenCoord scLine = getScreenCoord((Coord) { 0, y }, camera);
        DrawLine(
            0,
            scLine.y,
            camera.screenSize.x,
            scLine.y,
            BLACK
        );
    }
}
