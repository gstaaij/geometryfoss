#include "player.h"
#include "raylib.h"
#include "camera.h"
#include "object.h"
#include "ground.h"
#include <stddef.h>
#include <math.h>

// I got this value by measuring pixels
#define PLAYER_SPEED_X 303.46

// These values are completely incorrect, I need to change them
// I will probably make a GD mod to show me the velocity of the player at some point, so I can measure these values accurately
#define PLAYER_GRAVITY_Y 3054
#define PLAYER_JUMP_FORCE 620

void playerUpdate(Player* player, const DAObjects objects, const double deltaTime) {
    // This is always constant, so no need to change the velocity for this
    player->position.x += PLAYER_SPEED_X * deltaTime;

    if (player->isOnGround && (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(KEY_KP_5))) {
        player->velocity.y = PLAYER_JUMP_FORCE;
        player->isOnGround = false;
    }
    // I know using deltaTime like this isn't the most accurate, I should probably look at that Jonas Tyroller video about deltaTIme again
    player->position.y += player->velocity.y * deltaTime;
    player->velocity.y -= PLAYER_GRAVITY_Y * deltaTime;

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

void playerDraw(const Player player, bool drawHitboxes, const GDFCamera camera) {
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

    if (drawHitboxes) {
        // Outer hitbox
        hitboxDraw(player.outerHitbox, player.position, 1.0, OBJECT_HAZARD_HITBOX_COLOR, camera);
        // Inner hitbox
        hitboxDraw(player.innerHitbox, player.position, 1.0, OBJECT_SOLID_HITBOX_COLOR, camera);
    }
}

void playerDie(Player* player) {
    /// TODO: fancy animation
    playerReset(player);
}

void playerReset(Player* player) {
    player->position.x = 0;
    player->position.y = GROUND_Y + 15;
}
