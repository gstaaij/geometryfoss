#pragma once
#include "coord.h"
#include "object.h"
#include "hitbox.h"
#include <stdbool.h>

#define PLAYER_SIZE 30

typedef struct {
    Coord position;
    Coord velocity;
    double angle;

    bool isOnGround;
    bool isDead;

    double deadTime;
    
    Hitbox outerHitbox;
    Hitbox innerHitbox;
} Player;

// Player update loop
void playerUpdate(Player* player, const Object* objects, const double deltaTime);

// Draw the player
void playerDraw(const Player player, const GDFCamera camera);
// Draw the player's hitbox
void playerDrawHitboxes(const Player player, bool drawHitboxes, const GDFCamera camera);

// Kill the player
void playerDie(Player* player);

// Reset the player
void playerReset(Player* player);
