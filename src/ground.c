#include "ground.h"
#include "camera.h"

void drawGround(const Color color, const GDFCamera camera) {
    Coord groundCoord = {
        .x = 0, // x doesn't matter, the ground extends infinitely
        .y = GROUND_Y,
    };

    ScreenCoord scGround = getScreenCoord(groundCoord, camera);

    // We don't need to draw anything if the ground isn't on screen anyway
    if (scGround.y > camera.screenSize.y) return;

    // Draw the rectangle that forms the ground
    DrawRectangle(0, scGround.y < 0 ? 0 : scGround.y, camera.screenSize.x, camera.screenSize.y, color);
}
