#include "ground.h"
#include "camera.h"

void drawGround(const Color color, const GDFCamera camera) {
    Coord groundCoord = {
        .x = 0, // x doesn't matter, the ground extends infinitely
        .y = GROUND_Y,
    };

    ScreenCoord scGround = getScreenCoord(groundCoord, camera);

    if (scGround.y > camera.screenSize.y) return;

    DrawRectangle(0, scGround.y, camera.screenSize.x, camera.screenSize.y, color);
}
