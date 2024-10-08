#include "object.h"
#include "raylib.h"
#include "rlgl.h"
#include "camera.h"
#include "serialize.h"
#include <stdio.h>

void objectDraw(const Object object, const bool drawGlow, const GDFCamera camera) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefinitions[object.id];

    // Calculate the size of the block based on the Object Defenition and the Object's scale
    double scaleX = def.shape.scaleX * object.scale;
    double scaleY = def.shape.scaleY * object.scale;
    double blockSizeX = scaleX * 30;
    double blockSizeY = scaleY * 30;

    if (
        object.position.x + blockSizeX < camera.position.x - camera.screenSizeAsCoord.x/2 || object.position.x - blockSizeX > camera.position.x + camera.screenSizeAsCoord.x/2 ||
        object.position.y + blockSizeY < camera.position.y - camera.screenSizeAsCoord.y/2 || object.position.y - blockSizeY > camera.position.y + camera.screenSizeAsCoord.y/2
    ) {
        // Don't try to draw the block if it's out of screen
        return;
    }

    if (def.baseTextureMap.spriteSheet == NULL) {
        def.baseTextureMap = assetsTextureMap(def.baseTexturePath);
        if (def.detailTexturePath) def.detailTextureMap = assetsTextureMap(def.detailTexturePath);
        if (def.glowTexturePath) def.glowTextureMap = assetsTextureMap(def.glowTexturePath);
    }

    if (def.baseTextureMap.spriteSheet) {
        // Draw the texture

        if (drawGlow) {
            BeginBlendMode(BLEND_ADDITIVE);
                assetsDrawFromTextureMap(def.glowTextureMap, object.position, object.scale, object.angle, object.selected ? GREEN : ColorAlpha(WHITE, 0.5), camera);
            EndBlendMode();
        }
        assetsDrawFromTextureMap(def.detailTextureMap, object.position, object.scale, object.angle, object.selected ? GREEN : object.detailColor, camera);
        assetsDrawFromTextureMap(def.baseTextureMap, object.position, object.scale, object.angle, object.selected ? GREEN : object.baseColor, camera);
    } else {
        // Draw the shape

        // Convert some values to Screen Coordinates
        ScreenCoord scBlock = getScreenCoord(object.position, camera);
        double scBlockSizeX = convertToScreen(blockSizeX, camera);
        double scBlockSizeY = convertToScreen(blockSizeY, camera);
        double scHalfBlockSizeX = convertToScreen(blockSizeX / 2, camera);
        double scHalfBlockSizeY = convertToScreen(blockSizeY / 2, camera);
        double scBlockLineThick = convertToScreen(scaleX * 1.5, camera);

        rlPushMatrix();
        rlTranslatef(scBlock.x, scBlock.y, 0);
        rlRotatef(object.angle, 0, 0, 1);

        switch (def.shape.type) {
            case OBJSHAPE_NONE: {
                // Do nothing
            } break;
            case OBJSHAPE_BLOCK: {
                // Define a raylib Rectangle for the block
                Rectangle recBlock = {
                    .x = -scHalfBlockSizeX,
                    .y = -scHalfBlockSizeY,
                    .width = scBlockSizeX,
                    .height = scBlockSizeY,
                };
                // Draw a black square with a white outline
                DrawRectangleRec(recBlock, def.shape.color);
                DrawRectangleLinesEx(recBlock, scBlockLineThick, object.selected ? GREEN : object.baseColor);
            } break;
            case OBJSHAPE_SPIKE: {
                Vector2 vecSpikePoint1 = {
                    .x = 0,
                    .y = -scHalfBlockSizeY,
                };
                Vector2 vecSpikePoint2 = {
                    .x = -scHalfBlockSizeX,
                    .y =  scHalfBlockSizeY,
                };
                Vector2 vecSpikePoint3 = {
                    .x = scHalfBlockSizeX,
                    .y = scHalfBlockSizeY,
                };
                DrawTriangle(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, def.shape.color);
                // No defining thickness of lines :(
                DrawTriangleLines(vecSpikePoint1, vecSpikePoint2, vecSpikePoint3, object.selected ? GREEN : object.baseColor);
            } break;
        }

        rlPopMatrix();
    }
}

void objectDrawHitbox(const Object object, const bool drawHitbox, const GDFCamera camera) {
    if (!drawHitbox) return;

    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefinitions[object.id];

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

    if (cJSON_AddNumberToObject(objectJson, "baseColor", ColorToInt(object.baseColor)) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object base color");
        nob_return_defer(NULL);
    }

    if (cJSON_AddNumberToObject(objectJson, "detailColor", ColorToInt(object.detailColor)) == NULL) {
        TraceLog(LOG_ERROR, "Couldn't serialize object detail color");
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

    const cJSON* baseColorJson = cJSON_GetObjectItemCaseSensitive(objectJson, "baseColor");
    if (cJSON_IsNumber(baseColorJson)) {
        object->baseColor = GetColor((unsigned int) baseColorJson->valueint);
    } else {
        // Set a default color if the color wasn't set
        if (objectDefinitions[object->id].exists)
            object->baseColor = objectDefinitions[object->id].baseDefaultColor;
        else
            object->baseColor = WHITE;
    }

    const cJSON* detailColorJson = cJSON_GetObjectItemCaseSensitive(objectJson, "detailColor");
    if (cJSON_IsNumber(detailColorJson)) {
        object->detailColor = GetColor((unsigned int) detailColorJson->valueint);
    } else {
        // Set a default color if the color wasn't set
        if (objectDefinitions[object->id].exists)
            object->detailColor = objectDefinitions[object->id].detailDefaultColor;
        else
            object->detailColor = BLACK;
    }

    return true;
}

bool objectMouseOver(const Object object, const Coord clickPos) {
    // Get the Object Defenition tied to this Object
    ObjectDefinition def = objectDefinitions[object.id];

    if (clickPos.x > object.position.x - def.shape.scaleX * 15.0 && clickPos.x < object.position.x + def.shape.scaleX * 15.0) {
        return clickPos.y > object.position.y - def.shape.scaleY * 15.0 && clickPos.y < object.position.y + def.shape.scaleY * 15.0;
    }
    return false;
}
