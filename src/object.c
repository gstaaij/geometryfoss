#include "object.h"
#include "raylib.h"
#include "camera.h"
#include "serialize.h"
#include <stdio.h>

void objectDraw(const Object object, const GDFCamera camera) {
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

    if (
        // Don't divide the size by 2 because it's integer division
        scBlock.x + blockSize < 0 || scBlock.x - blockSize > camera.screenSize.x ||
        scBlock.y + blockSize < 0 || scBlock.y - blockSize > camera.screenSize.y
    ) {
        return;
    }

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
        if (object.selected) {
            BeginBlendMode(BLEND_MULTIPLIED);
                DrawRectangleRec(recBlock, GREEN);
            EndBlendMode();
        }
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
        if (object.selected) {
            BeginBlendMode(BLEND_MULTIPLIED);
                DrawTriangle(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, GREEN);
            EndBlendMode();
        }
        break;
    }
}

void objectDrawHitbox(const Object object, const bool drawHitbox, const GDFCamera camera) {
    if (!drawHitbox) return;

    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    if (def.type != OBJECT_NONSOLID) {
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

Nob_String_Builder objectSerialize(const Object object, const int tabSize) {
    Nob_String_Builder objectJson = {0};
    nob_sb_append_cstr(&objectJson, "{\n");

    // Position
    serializeTAB(&objectJson, tabSize + 1);
    serializePROPERTY(&objectJson, "position");
    Nob_String_Builder posJson = coordSerialize(object.position, tabSize + 1);
    nob_sb_append_buf(&objectJson, posJson.items, posJson.count);
    nob_sb_free(posJson);
    nob_sb_append_cstr(&objectJson, ",\n");

    // Angle
    serializeTAB(&objectJson, tabSize + 1);
    serializePROPERTY(&objectJson, "angle");
    nob_sb_append_cstr(&objectJson, nob_temp_sprintf("%lf", object.angle));
    nob_sb_append_cstr(&objectJson, ",\n");

    // Scale
    serializeTAB(&objectJson, tabSize + 1);
    serializePROPERTY(&objectJson, "scale");
    nob_sb_append_cstr(&objectJson, nob_temp_sprintf("%lf", object.scale));
    nob_sb_append_cstr(&objectJson, ",\n");

    // ID
    serializeTAB(&objectJson, tabSize + 1);
    serializePROPERTY(&objectJson, "id");
    nob_sb_append_cstr(&objectJson, nob_temp_sprintf("%d", object.id));
    nob_da_append(&objectJson, '\n');

    serializeTAB(&objectJson, tabSize);
    nob_da_append(&objectJson, '}');

    return objectJson;
}

bool objectMouseOver(const Object object, const Coord clickPos) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    if (clickPos.x > object.position.x - def.shape.scale * 15.0 && clickPos.x < object.position.x + def.shape.scale * 15.0) {
        return clickPos.y > object.position.y - def.shape.scale * 15.0 && clickPos.y < object.position.y + def.shape.scale * 15.0;
    }
    return false;
}
