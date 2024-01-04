#include "player.h"
#include "raylib.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include <stddef.h>
#include <math.h>

// I got this value using the 2.2 info label
// #define PLAYER_SPEED_X 311.63
// And I got this value by testing the stairs
#define PLAYER_SPEED_X 323.6

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

    // If we are on the ground, and we press one of the jump keys or click with the mouse, jump
    if (player->isOnGround && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_KP_5) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))) {
        player->velocity.y = PLAYER_JUMP_FORCE;
        player->isOnGround = false;
    }
    // Do exactly what was done in the Jonas Tyroller video, because even though my previous method provided the same results, this should be a little bit faster maybe I think
    double halfAcceleration = PLAYER_GRAVITY_Y * deltaTime * 0.5;
    // Add half the gravity to the velocity
    player->velocity.y -= halfAcceleration;
    // The player y velocity is now the average speed of last frame
    player->position.y += player->velocity.y * deltaTime;
    // Add half the gravity to the velocity again
    player->velocity.y -= halfAcceleration;

    // Go over all objects to check for collisions
    for (size_t i = 0; i < objects.count; ++i) {
        Object object = objects.items[i];
        ObjectDefinition def = objectDefenitions[object.id];

        switch(def.type) {
        case OBJECT_SOLID:
            // If the inner hitbox of the player collides with the hitbox of the object, kill the player
            if (hitboxCollides(player->innerHitbox, player->position, def.hitbox, object.position))
                playerDie(player);

            if (hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position)) {
                // If the player is above the object, is going downwards, and the inner hitbox isn't in line with the object's hitbox, snap the player to the top of the object
                if (
                    player->position.y > object.position.y &&
                    player->velocity.y < 0 &&
                    !hitboxSquareCollidesOnlyY(player->innerHitbox, player->position, def.hitbox, object.position)
                ) {
                    // Go up until you don't hit the object anymore
                    while (hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position)) {
                        player->position.y += 3;
                    }
                    // Go down in smaller steps until you hit the object again
                    while (!hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position)) {
                        player->position.y -= 0.1;
                    }
                    // Reset the y position and set isOnGround to true
                    player->velocity.y = 0;
                    player->isOnGround = true;
                }
            } else {
                // If you are underneath the ground level, you are grounded, otherwise, you aren't.
                if (player->position.y < GROUND_Y + (PLAYER_SIZE / 2)) {
                    player->velocity.y = 0;
                    player->position.y = GROUND_Y + (PLAYER_SIZE / 2);
                    player->isOnGround = true;
                } else if (player->wasOnGround) {
                    player->isOnGround = false;
                }
            }
            break;
        case OBJECT_HAZARD:
            // Die if the outer hitbox hits the hitbox of the hazard
            if (hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position))
                playerDie(player);
            break;
        default:
            /// TODO: handle the other object types
            break;
        }
    }

    // If the player is not on the ground, rotate
    if (!player->isOnGround && !player->wasOnGround) {
        player->angle += 360.0 * deltaTime;
        while (player->angle >= 360)
            player->angle -= 360;
    } else {
        // Otherwise, snap back
        /// TODO: ease this
        player->angle = round(player->angle / 90) * 90;
    }

    player->wasOnGround = player->isOnGround;
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
