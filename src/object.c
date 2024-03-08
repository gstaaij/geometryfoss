#include "object.h"
#include "raylib.h"
#include "rlgl.h"
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
    long scHalfBlockSize = convertToScreen(blockSize / 2, camera);
    long scBlockLineThick = convertToScreen(scale * 1.5, camera);

    if (
        // Don't divide the size by 2 because it's integer division
        scBlock.x + blockSize < 0 || scBlock.x - blockSize > camera.screenSize.x ||
        scBlock.y + blockSize < 0 || scBlock.y - blockSize > camera.screenSize.y
    ) {
        // Don't try to draw the block if it's out of screen
        return;
    }

    rlPushMatrix();
    rlTranslatef(scBlock.x, scBlock.y, 0);
    rlRotatef(object.angle, 0, 0, 1);

    switch (def.shape.type) {
        case OBJSHAPE_BLOCK: {
            // Define a raylib Rectangle for the block
            Rectangle recBlock = {
                .x = -scHalfBlockSize,
                .y = -scHalfBlockSize,
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
        } break;
        case OBJSHAPE_SPIKE: {
            Vector2 vecSpikePoint1 = {
                .x = 0,
                .y = -scHalfBlockSize,
            };
            Vector2 vecSpikePoint2 = {
                .x = -scHalfBlockSize,
                .y =  scHalfBlockSize,
            };
            Vector2 vecSpikePoint3 = {
                .x = scHalfBlockSize,
                .y = scHalfBlockSize,
            };
            DrawTriangle(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, BLACK);
            // No defining thickness of lines :(
            DrawTriangleLines(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, WHITE);

            if (object.selected) {
                BeginBlendMode(BLEND_MULTIPLIED);
                    DrawTriangle(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, GREEN);
                EndBlendMode();
            }
        } break;
    }

    rlPopMatrix();
}

void objectDrawHitbox(const Object object, const bool drawHitbox, const GDFCamera camera) {
    if (!drawHitbox) return;

    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    if (def.type != OBJECT_NONSOLID) {
        // Determine the hitbox color
        Color hitboxColor;
        switch (def.type) {
            case OBJECT_SOLID: {
                hitboxColor = OBJECT_SOLID_HITBOX_COLOR;
            } break;
            case OBJECT_HAZARD: {
                hitboxColor = OBJECT_HAZARD_HITBOX_COLOR;
            } break;
            case OBJECT_PORTAL: {
                hitboxColor = OBJECT_PORTAL_HITBOX_COLOR;
            } break;
            case OBJECT_PAD: {
                hitboxColor = OBJECT_PAD_HITBOX_COLOR;
            } break;
            case OBJECT_RING: {
                hitboxColor = OBJECT_RING_HITBOX_COLOR;
            } break;
            default: {
                hitboxColor = WHITE;
            } break;
        }

        // Call the hitboxDraw function to do the work for us
        hitboxDraw(def.hitbox, object.position, object.scale, object.angle, hitboxColor, camera);
    }
}

cJSON* objectSerialize(const Object object) {

    cJSON* objectJson = cJSON_CreateObject();

    cJSON* result = objectJson;

    cJSON* posJson = coordSerialize(object.position);
    if (posJson == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object position");
        nob_return_defer(NULL);
    }
    cJSON_AddItemToObject(objectJson, "position", posJson);

    if (cJSON_AddNumberToObject(objectJson, "angle", object.angle) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object angle");
        nob_return_defer(NULL);
    }

    if (cJSON_AddNumberToObject(objectJson, "scale", object.scale) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object scale");
        nob_return_defer(NULL);
    }

    if (cJSON_AddNumberToObject(objectJson, "id", object.id) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object id");
        nob_return_defer(NULL);
    }

defer:
    if (result == NULL)
        cJSON_Delete(objectJson);
    return result;
}

bool objectDeserialize(Object* object, const cJSON* objectJson) {
    const cJSON* posJson = cJSON_GetObjectItemCaseSensitive(objectJson, "position");
    if (cJSON_IsObject(posJson)) {
        Coord newPosition = {0};
        if (!coordDeserialize(&newPosition, posJson))
            TraceLog(LOG_WARNING, "Failed to parse object position, skipping...");
        object->position = newPosition;
    }

    const cJSON* angleJson = cJSON_GetObjectItemCaseSensitive(objectJson, "angle");
    if (cJSON_IsNumber(angleJson)) {
        object->angle = angleJson->valuedouble;
    }

    const cJSON* scaleJson = cJSON_GetObjectItemCaseSensitive(objectJson, "scale");
    if (cJSON_IsNumber(scaleJson)) {
        object->scale = scaleJson->valuedouble;
    }

    const cJSON* idJson = cJSON_GetObjectItemCaseSensitive(objectJson, "id");
    if (cJSON_IsNumber(idJson)) {
        object->id = idJson->valueint;
    }

    return true;
}

bool objectMouseOver(const Object object, const Coord clickPos) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefenitions[object.id];

    if (clickPos.x > object.position.x - def.shape.scale * 15.0 && clickPos.x < object.position.x + def.shape.scale * 15.0) {
        return clickPos.y > object.position.y - def.shape.scale * 15.0 && clickPos.y < object.position.y + def.shape.scale * 15.0;
    }
    return false;
}
