#include "player.h"
#include "raylib.h"
#include "stb_ds.h"
#include "camera.h"
#include "constants/player.h"
#include "object.h"
#include "ground.h"
#include <stddef.h>
#include <math.h>

#define SOLID_COLLISION_TPS 60
long double timer = 0;

void playerUpdate(Player* player, const Object* objects, const double deltaTime) {
    if (player->isDead) {
        player->deadTime += deltaTime;
        if (player->deadTime > 1.0) {
            playerReset(player);
        }
        return;
    }

    player->timeAlive += deltaTime;
    // Lock the snapping up a block to 60 TPS to fix some physics related issues
    timer += deltaTime;
    bool shouldDoSnapUp = false;
    if (timer >= 1.0 / SOLID_COLLISION_TPS) {
        timer -= 1.0 / SOLID_COLLISION_TPS;
        shouldDoSnapUp = true;
    }

    // This is always constant, so no need to change the velocity for this
    player->velocity.x = PLAYER_NORMAL_SPEED;
    player->position.x += player->velocity.x * deltaTime;

    // If we are on the ground, and we press one of the jump keys or click with the mouse, jump
    bool jump = player->isOnGround && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_KP_5) || IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    if (jump) {
        player->velocity.y = PLAYER_JUMP_FORCE;
        player->isOnGround = false;
    }

    // Do exactly what was done in the Jonas Tyroller video
    double halfAcceleration = PLAYER_GRAVITY_FORCE * deltaTime * 0.5;
    // Add half the gravity to the velocity
    player->velocity.y -= halfAcceleration;
    // The player y velocity is now the average speed of last frame
    player->position.y += player->velocity.y * deltaTime;
    // Add half the gravity to the velocity again
    player->velocity.y -= halfAcceleration;

    // Go over all objects to check for collisions
    bool onBlock = false;
    for (size_t i = 0; i < arrlenu(objects); ++i) {
        Object object = objects[i];
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
                    bool shouldBeGrounded = true;
                    if (shouldDoSnapUp) {
                        // This part is the reason for the inconsistency of the physics depending on the framerate
                        // That's why we lock it to 60 TPS
                        // The reason is that on 60 TPS the player goes farther into the block before it snaps up and
                        // can jump again, so if we lock it to 60 TPS, every TPS of 60 or above will be (more or less) consistent
                        
                        // Go up until you don't hit the object anymore
                        while (hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position)) {
                            player->position.y += 1;
                        }
                        // Go down in smaller steps until you hit the object again
                        while (!hitboxCollides(player->outerHitbox, player->position, def.hitbox, object.position)) {
                            player->position.y -= 0.01;
                        }
                    } else if (
                        !hitboxSquareCollidesOnlyX(
                            player->outerHitbox, (Coord) {
                                // The x position minus the maximum distance the player can be into a block without having been snapped back to the top
                                player->position.x - (1.0 / SOLID_COLLISION_TPS * player->velocity.x + 0.01), // The +0.01 is for rounding errors
                                player->position.y,
                            },
                            def.hitbox, object.position
                        )
                    ) {
                        // Only set the y velocity and isOnGround if the player isn't potentially going to be snapped to the top of the block
                        shouldBeGrounded = false;
                    }

                    if (shouldBeGrounded) {
                        // Reset the y velocity and set isOnGround to true 
                        player->velocity.y = 0;
                        player->isOnGround = true;
                        onBlock = true;
                    }
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

    
    if (!onBlock) {
        // If you are underneath the ground level, you are grounded, otherwise, you aren't.
        if (player->position.y < GROUND_Y + (PLAYER_SIZE / 2)) {
            player->velocity.y = 0;
            player->position.y = GROUND_Y + (PLAYER_SIZE / 2);
            player->isOnGround = true;
        } else {
            player->isOnGround = false;
        }
    }

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
