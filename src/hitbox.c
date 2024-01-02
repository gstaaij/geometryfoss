#include "hitbox.h"
#include "camera.h"
#include "util.h"
#include <stdio.h>

void hitboxDraw(const Hitbox hitbox, const Coord parentPosition, const double scale, const Color hitboxColor, const GDFCamera camera) {
    if (hitbox.shape == SQUARE) {
        // Calculate the absolute position of the hitbox
        Coord hitboxCoord = {
            .x = parentPosition.x + hitbox.offset.x,
            .y = parentPosition.y + hitbox.offset.y,
        };

        // Convert the width, height and line thickness of the hitbox to screen pixels
        long scHitboxWidth = convertToScreen(hitbox.width * scale, camera);
        long scHitboxHeight = convertToScreen(hitbox.height * scale, camera);
        long scHitboxLineThick = convertToScreen(1, camera);

        // Convert the absolute position to Screen Coordinates
        ScreenCoord scHitbox = getScreenCoord(hitboxCoord, camera);

        // Define a raylib Rectangle for the hitbox
        Rectangle recHitbox = {
            .x = scHitbox.x - (scHitboxWidth / 2),
            .y = scHitbox.y - (scHitboxHeight / 2),
            .width = scHitboxWidth,
            .height = scHitboxHeight,
        };
        // Draw the hitbox
        DrawRectangleLinesEx(recHitbox, scHitboxLineThick, hitboxColor);
    }
}

bool hitboxCollides(const Hitbox hitbox1, const Coord parent1Position, const Hitbox hitbox2, const Coord parent2Position) {
    double dx = (parent2Position.x + hitbox2.offset.x) - (parent1Position.x + hitbox1.offset.x);
    double dy = (parent2Position.y + hitbox2.offset.y) - (parent1Position.y + hitbox1.offset.y);

    double halfWidths = (hitbox1.width / 2) + (hitbox2.width / 2);
    double halfHeights = (hitbox1.height / 2) + (hitbox2.height / 2);

    if (dabs(dx) < halfWidths) {
        return dabs(dy) < halfHeights;
    }

    return false;
}
