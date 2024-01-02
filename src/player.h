#pragma once
#include "dynamicarrays.h"
#include "coord.h"
#include "hitbox.h"
#include <stdbool.h>

#define PLAYER_SIZE 30

typedef struct {
    Coord position;
    Coord velocity;
    bool isOnGround;
    double angle;
    Hitbox outerHitbox;
    Hitbox innerHitbox;
} Player;

void playerUpdate(Player* player, const DAObjects objects, const double deltaTime);

void playerDraw(const Player player, bool drawHitboxes, const GDFCamera camera);

void playerDie(Player* player);

void playerReset(Player* player);
