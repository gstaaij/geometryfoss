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
    case OBJSHAPE_BLOCK:
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
    case OBJSHAPE_SPIKE:
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

    if (drawHitbox && def.type != OBJECT_NONSOLID) {
        // Determine the hitbox color
        Color hitboxColor;
        switch (def.type) {
        case OBJECT_SOLID:
            hitboxColor = OBJECT_SOLID_HITBOX_COLOR;
            break;
        case OBJECT_HAZARD:
            hitboxColor = OBJECT_HAZARD_HITBOX_COLOR;
            break;
        case OBJECT_PORTAL:
            hitboxColor = OBJECT_PORTAL_HITBOX_COLOR;
            break;
        case OBJECT_PAD:
            hitboxColor = OBJECT_PAD_HITBOX_COLOR;
            break;
        case OBJECT_RING:
            hitboxColor = OBJECT_RING_HITBOX_COLOR;
            break;
        default:
            hitboxColor = WHITE;
            break;
        }

        // Call the hitboxDraw function to do the work for us
        hitboxDraw(def.hitbox, object.position, object.scale, hitboxColor, camera);
    }
}
