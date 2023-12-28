#include "nob.h"
#include "raylib.h"
#include "object.h"

void objectDraw(const Object object, const bool drawHitbox, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    // Draw the shape

    if (def.shape.type == BLOCK) {
        // Calculate the size of the block based on the Object Defenition and the Object's scale
        double scale = def.shape.scale * object.scale;
        double blockSize = scale * 30;

        // Convert some values to Screen Coordinates
        ScreenCoord scBlock = getScreenCoord(object.position, cameraCoord, screenSizeAsCoord, screenSize);
        long scBlockSize = convertToScreen(blockSize, screenSizeAsCoord, screenSize);
        long scBlockLineThick = convertToScreen(scale * 1.5, screenSizeAsCoord, screenSize);

        // Define a raylib Rectangle for the block
        Rectangle recBlock = {
            .x = scBlock.x - (scBlockSize / 2),
            .y = scBlock.y - (scBlockSize / 2),
            .width = scBlockSize,
            .height = scBlockSize,
        };
        // Draw a black square with a white outline
        DrawRectangleRec(recBlock, BLACK);
        DrawRectangleLinesEx(recBlock, scBlockLineThick, WHITE);
    }

    // Draw the hitbox

    if (drawHitbox && def.type != NONSOLID) {
        // Determine the hitbox color
        Color hitboxColor;
        switch (def.type) {
        case SOLID:
            hitboxColor = GetColor(0x0000ffff);
            break;
        case HAZARD:
            hitboxColor = GetColor(0xff0000ff);
            break;
        case PORTAL:
        case PAD:
        case RING:
            hitboxColor = GetColor(0x00ff00ff);
            break;
        default:
            hitboxColor = WHITE;
            break;
        }

        if (def.hitbox.shape == SQUARE) {
            // Calculate the absolute position of the hitbox
            Coord hitboxCoord = {
                .x = object.position.x + def.hitbox.offset.x,
                .y = object.position.y + def.hitbox.offset.y,
            };

            // Convert the width, height and line thickness of the hitbox to screen pixels
            long scHitboxWidth = convertToScreen(def.hitbox.width * object.scale, screenSizeAsCoord, screenSize);
            long scHitboxHeight = convertToScreen(def.hitbox.height * object.scale, screenSizeAsCoord, screenSize);
            long scHitboxLineThick = convertToScreen(1, screenSizeAsCoord, screenSize);

            // Convert the absolute position to Screen Coordinates
            ScreenCoord scHitbox = getScreenCoord(hitboxCoord, cameraCoord, screenSizeAsCoord, screenSize);

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
}
