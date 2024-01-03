#include "player.h"
#include "raylib.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include <stddef.h>
#include <math.h>

// I got this value using the 2.2 info label
#define PLAYER_SPEED_X 311.63

// These two values were found using trail and error knowing that:
// - The lowest possible block to survive a jump under is three blocks in above the ground minus 5.5 coordinate points on the y axis
// - The hardest possible triple spike you can survive has the last spike offset by +13 coordinate points on the x axis
// Given that they're round values, they probably are the real values, but I will have to confirm that once I get
// around to making a GD mod to show me or once hacks come to GD 2.2
#define PLAYER_GRAVITY_Y 2800
#define PLAYER_JUMP_FORCE 600

void playerUpdate(Player* player, const DAObjects objects, const double deltaTime) {
    if (player->isDead) {
        player->deadTime += deltaTime;
        if (player->deadTime > 1.0) {
            playerReset(player);
        }
        return;
    }

    // This is always constant, so no need to change the velocity for this
    player->position.x += PLAYER_SPEED_X * deltaTime;

    if (player->isOnGround && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_KP_5))) {
        player->velocity.y = PLAYER_JUMP_FORCE;
        player->isOnGround = false;
    }
    // Using deltaTime like this is probably more accurate, but I should still look at the Jonas Tyroller video about deltaTIme again
    double halfDeltaTime = deltaTime * 0.5;
    player->position.y += player->velocity.y * halfDeltaTime;
    player->velocity.y -= PLAYER_GRAVITY_Y * deltaTime;
    player->position.y += player->velocity.y * halfDeltaTime;

    if (player->position.y < GROUND_Y + (PLAYER_SIZE / 2)) {
        player->velocity.y = 0;
        player->position.y = GROUND_Y + (PLAYER_SIZE / 2);
        player->isOnGround = true;
    }

    if (!player->isOnGround) {
        player->angle += 360.0 * deltaTime;
        while (player->angle >= 360)
            player->angle -= 360;
    } else {
        player->angle = round(player->angle / 90) * 90;
    }

    for (size_t i = 0; i < objects.count; ++i) {
        Object object = objects.items[i];
        ObjectDefinition def = objectDefenitions[object.id];
        switch(def.type) {
        case OBJECT_SOLID:
            if (hitboxCollides(player->innerHitbox, player->position, def.hitbox, object.position))
                playerDie(player);
            break;
        case OBJECT_HAZARD:
            if (hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position))
                playerDie(player);
            break;
        default:
            /// TODO: handle the other object types
            break;
        }
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
}
