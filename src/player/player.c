#include "player.h"
#include "physics/playerphysics.h"
#include "raylib.h"
#include "stb_ds.h"
#include "camera.h"
#include "constants/player.h"
#include "object.h"
#include "ground.h"
#include <stddef.h>
#include <math.h>

void playerUpdate(Player* player, const Object* objects, const double deltaTime) {
    if (player->isDead) {
        player->deadTime += deltaTime;
        if (player->deadTime > 1.0) {
            playerReset(player);
        }
        return;
    }

    playerphysicsUpdate(player, objects, deltaTime);

    // If the player is not on the ground, rotate
    if (!player->isOnGround) {
        player->angle += 360.0 * deltaTime;
        while (player->angle >= 360)
            player->angle -= 360;
    } else {
        // Otherwise, snap back
        /// TODO: ease this
        player->angle = round(player->angle / 90) * 90;
    }
}

void playerDraw(const Player player, const GDFCamera camera) {
    // Convert to screen coordinates
    ScreenCoord scPlayer = getScreenCoord(player.position, camera);
    long scPlayerSize = convertToScreen(PLAYER_SIZE, camera);
    // Create a rectangle and a vector2 for the position & size, and the rotation anchor point respectively
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
    // Draw a rectangle with the player angle
    DrawRectanglePro(playerRect, playerCenter, player.angle, GREEN);
}

void playerDrawHitboxes(const Player player, bool drawHitboxes, const GDFCamera camera) {
    if (!drawHitboxes) return;
    // Outer hitbox
    hitboxDraw(player.outerHitbox, player.position, 1.0, OBJECT_HAZARD_HITBOX_COLOR, camera);
    // Inner hitbox
    hitboxDraw(player.innerHitbox, player.position, 1.0, OBJECT_SOLID_HITBOX_COLOR, camera);
}

void playerDie(Player* player) {
    /// TODO: explosion
    player->isDead = true;
    player->deadTime = 0;
}

void playerReset(Player* player) {
    // Reset position
    player->position.x = 0;
    player->position.y = GROUND_Y + 15;
    // Reset velocity
    player->velocity.x = player->velocity.y = 0;
    // Reset dead
    player->isDead = false;
    player->deadTime = 0;

    player->timeAlive = 0.0;
    timer = 0;
}
