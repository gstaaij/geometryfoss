#include "playerphysics.h"
#include "lib/stb/stb_ds.h"
#include "constants/player.h"
#include "constants/level.h"

void playerphysicsUpdate(Player* player, const Object* objects, const bool shouldDoSnapUp) {
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
}
