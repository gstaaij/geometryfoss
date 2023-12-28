#include "nob.h"
#include "raylib.h"
#include "object.h"

void objectDraw(const Object object, const bool drawHitbox, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize) {
    ObjectDefinition def = objectDefenitions[object.id];
    if (def.shape.type == BLOCK) {
        double scale = def.shape.scale * object.scale;
        double blockSize = scale * 30;

        ScreenCoord scBlock = getScreenCoord(object.position, cameraCoord, screenSizeAsCoord, screenSize);
        long scBlockSize = convertToScreen(blockSize, screenSizeAsCoord, screenSize);
        long scBlockLineThick = convertToScreen(scale * 1.5, screenSizeAsCoord, screenSize);

        Rectangle recBlock = {
            .x = scBlock.x - (scBlockSize / 2),
            .y = scBlock.y - (scBlockSize / 2),
            .width = scBlockSize,
            .height = scBlockSize,
        };
        DrawRectangleRec(recBlock, BLACK);
        DrawRectangleLinesEx(recBlock, scBlockLineThick, WHITE);
    }

    if (drawHitbox && def.type != NONSOLID) {
        Color hitboxColor = WHITE;
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
            break;
        }

        if (def.hitbox.shape == SQUARE) {
            Coord hitboxCoord = {
                .x = object.position.x + def.hitbox.offset.x,
                .y = object.position.y + def.hitbox.offset.y,
            };

            long scHitboxWidth = convertToScreen(def.hitbox.width * object.scale, screenSizeAsCoord, screenSize);
            long scHitboxHeight = convertToScreen(def.hitbox.height * object.scale, screenSizeAsCoord, screenSize);

            ScreenCoord scHitbox = getScreenCoord(hitboxCoord, cameraCoord, screenSizeAsCoord, screenSize);
            long scHitboxLineThick = convertToScreen(1, screenSizeAsCoord, screenSize);

            Rectangle recHitbox = {
                .x = scHitbox.x - (scHitboxWidth / 2),
                .y = scHitbox.y - (scHitboxHeight / 2),
                .width = scHitboxWidth,
                .height = scHitboxHeight,
            };
            DrawRectangleLinesEx(recHitbox, scHitboxLineThick, hitboxColor);
        }
    }
}
