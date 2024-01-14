#include "grid.h"
#include "raylib.h"
#include "math.h"

void gridDraw(const GDFCamera camera) {
    double left = camera.position.x - camera.screenSizeAsCoord.x / 2;
    double bottom = camera.position.y - camera.screenSizeAsCoord.y / 2;
    
    double startX = floor(left / GRID_WIDTH) * GRID_WIDTH;
    double startY = floor(bottom / GRID_HEIGHT) * GRID_HEIGHT;

    double endX = camera.position.x + camera.screenSizeAsCoord.x / 2;
    double endY = camera.position.y + camera.screenSizeAsCoord.y / 2;

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
