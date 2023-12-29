#include "player.h"
#include "raylib.h"
#include "camera.h"
#include "object.h"

void playerDraw(const Player player, bool drawHitboxes, const GDFCamera camera) {
    ScreenCoord scPlayer = getScreenCoord(player.position, camera);
    long scPlayerSize = convertToScreen(PLAYER_SIZE, camera);
    Rectangle playerRect = {
        .x = scPlayer.x,
        .y = scPlayer.y,
        .width = scPlayerSize,
        .height = scPlayerSize,
    };
    Vector2 playerCenter = {
        .x = scPlayerSize / 2,
        .y = scPlayerSize / 2,
    };
    DrawRectanglePro(playerRect, playerCenter, player.angle, GREEN);

    if (drawHitboxes) {
        // Outer hitbox
        hitboxDraw(player.outerHitbox, player.position, 1.0, HAZARD_OBJECT_HITBOX_COLOR, camera);
        // Inner hitbox
        hitboxDraw(player.innerHitbox, player.position, 1.0, SOLID_OBJECT_HITBOX_COLOR, camera);
    }
}
