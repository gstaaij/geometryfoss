#include "object.h"
#include "nob.h"
#include "raylib.h"
#include "camera.h"
#include <stdio.h>

void objectDraw(const Object object, const bool drawHitbox, const GDFCamera camera) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    // Draw the shape

    // Calculate the size of the block based on the Object Defenition and the Object's scale
    double scale = def.shape.scale * object.scale;
    double blockSize = scale * 30;

    // Convert some values to Screen Coordinates
    ScreenCoord scBlock = getScreenCoord(object.position, camera);
    long scBlockSize = convertToScreen(blockSize, camera);
    long scBlockLineThick = convertToScreen(scale * 1.5, camera);

    switch (def.shape.type) {
    case BLOCK:
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
        break;
    case SPIKE:
        Vector2 vecSpikePoint1 = {
            .x = scBlock.x,
            .y = scBlock.y - (scBlockSize / 2),
        };
        Vector2 vecSpikePoint2 = {
            .x = scBlock.x - (scBlockSize / 2),
            .y = scBlock.y + (scBlockSize / 2),
        };
        Vector2 vecSpikePoint3 = {
            .x = scBlock.x + (scBlockSize / 2),
            .y = scBlock.y + (scBlockSize / 2),
        };
        DrawTriangle(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, BLACK);
        DrawTriangleLines(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, WHITE);
        break;
    }

    // Draw the hitbox

    if (drawHitbox && def.type != NONSOLID) {
        // Determine the hitbox color
        Color hitboxColor;
        switch (def.type) {
        case SOLID:
            hitboxColor = SOLID_OBJECT_HITBOX_COLOR;
            break;
        case HAZARD:
            hitboxColor = HAZARD_OBJECT_HITBOX_COLOR;
            break;
        case PORTAL:
        case PAD:
        case RING:
            hitboxColor = PORTAL_OBJECT_HITBOX_COLOR;
            break;
        default:
            hitboxColor = WHITE;
            break;
        }

        hitboxDraw(def.hitbox, object.position, object.scale, hitboxColor, camera);
    }
}
