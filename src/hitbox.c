#include "hitbox.h"
#include "camera.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

bool shouldSwapWidthAndHeight(const int hitbox1Angle, const int hitbox2Angle) {
    return  (hitbox1Angle % 180 == 90 && hitbox2Angle % 180 ==  0) ||
            (hitbox1Angle % 180 ==  0 && hitbox2Angle % 180 == 90);
}


void hitboxDraw(const Hitbox hitbox, const Coord parentPosition, const double scale, const int angle, const Color hitboxColor, const GDFCamera camera) {
    if (hitbox.shape == HITBOX_SQUARE) {
        // Calculate the absolute position of the hitbox
        Coord hitboxCoord = {
            .x = parentPosition.x + hitbox.offset.x,
            .y = parentPosition.y + hitbox.offset.y,
        };

        // Convert the width, height and line thickness of the hitbox to screen pixels
        long scHitboxWidth = convertToScreen(hitbox.width * scale, camera);
        long scHitboxHeight = convertToScreen(hitbox.height * scale, camera);
        if (shouldSwapWidthAndHeight(hitboxGetRealAngle(hitbox, angle), 0)) {
            long tmp = scHitboxWidth;
            scHitboxWidth = scHitboxHeight;
            scHitboxHeight = tmp;
        }
        long scHitboxLineThick = convertToScreen(1, camera);

        // Convert the absolute position to Screen Coordinates
        ScreenCoord scHitbox = getScreenCoord(hitboxCoord, camera);

        if (
            // Don't divide the width and height by 2 because it's integer division
            scHitbox.x + scHitboxWidth  < 0 || scHitbox.x - scHitboxWidth  > camera.screenSize.x ||
            scHitbox.y + scHitboxHeight < 0 || scHitbox.y - scHitboxHeight > camera.screenSize.y
        ) {
            return;
        }

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

int hitboxGetRealAngle(const Hitbox hitbox, const int angle) {
    if (hitbox.rotationMode == HITBOX_ROTATION_MODE_NONE) {
        return 0;
    }
    if (hitbox.rotationMode == HITBOX_ROTATION_MODE_FULL) {
        TraceLog(LOG_FATAL, "NOT IMPLEMENTED: HITBOX_ROTATION_MODE_FULL");
        return 0;
    }
    return (int) (round((double) angle / 90) * 90);
}

bool hitboxCollides(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle) {
    if (hitbox1.shape == HITBOX_SQUARE && hitbox2.shape == HITBOX_SQUARE) {
        // Calculate the distance between the hitboxes in the two axes
        double dx = (parent2Position.x + hitbox2.offset.x) - (parent1Position.x + hitbox1.offset.x);
        double dy = (parent2Position.y + hitbox2.offset.y) - (parent1Position.y + hitbox1.offset.y);

        // Add half of the width of hitbox 1 to half of the width of hitbox 2
        double halfWidths = (hitbox1.width / 2) + (hitbox2.width / 2);
        // Do the same for the heights
        double halfHeights = (hitbox1.height / 2) + (hitbox2.height / 2);
        if (shouldSwapWidthAndHeight(hitboxGetRealAngle(hitbox1, hitbox1Angle), hitboxGetRealAngle(hitbox2, hitbox2Angle))) {
            double tmp = halfWidths;
            halfWidths = halfHeights;
            halfHeights = tmp;
        }

        // If the x distance is smaller than the combined half widths, we are colliding on the x axis
        if (dabs(dx) < halfWidths) {
            // If then the y distance is also smaller than the combined half widths, we are colliding on both axes
            return dabs(dy) < halfHeights;
        }
    }

    // Square to circle collision isn't implemented yet
    if (hitbox1.shape == HITBOX_SQUARE && hitbox2.shape == HITBOX_CIRCLE) {
        assert("NOT IMPLEMENTED: square to circle collision" && false);
    }

    // We aren't colliding or we haven't implemented collision for these two hitbox shapes yet
    return false;
}

bool hitboxSquareCollidesOnlyX(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle) {
    if (hitbox1.shape != HITBOX_SQUARE || hitbox2.shape != HITBOX_SQUARE) {
        assert("INCOMPATIBLE: this function only works with square hitboxes" && false);
    }

    // Calculate the distance between the hitboxes in the two axes
    double dx = (parent2Position.x + hitbox2.offset.x) - (parent1Position.x + hitbox1.offset.x);

    // Add half of the width of hitbox 1 to half of the width of hitbox 2
    double halfWidths = (hitbox1.width / 2) + (hitbox2.width / 2);
    if (shouldSwapWidthAndHeight(hitboxGetRealAngle(hitbox1, hitbox1Angle), hitboxGetRealAngle(hitbox2, hitbox2Angle))) {
        halfWidths = (hitbox1.height / 2) + (hitbox2.height / 2);
    }

    // If the x distance is smaller than the combined half widths, we are colliding on the x axis
    return dabs(dx) < halfWidths;
}

bool hitboxSquareCollidesOnlyY(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle) {
    if (hitbox1.shape != HITBOX_SQUARE || hitbox2.shape != HITBOX_SQUARE) {
        assert("INCOMPATIBLE: this function only works with square hitboxes" && false);
    }

    // Calculate the distance between the hitboxes in the two axes
    double dy = (parent2Position.y + hitbox2.offset.y) - (parent1Position.y + hitbox1.offset.y);

    // Add half of the height of hitbox 1 to half of the height of hitbox 2
    double halfHeights = (hitbox1.height / 2) + (hitbox2.height / 2);
    if (shouldSwapWidthAndHeight(hitboxGetRealAngle(hitbox1, hitbox1Angle), hitboxGetRealAngle(hitbox2, hitbox2Angle))) {
        halfHeights = (hitbox1.width / 2) + (hitbox2.width / 2);
    }

    // If the y distance is smaller than the combined half heights, we are colliding on the y axis
    return dabs(dy) < halfHeights;
}
