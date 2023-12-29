#pragma once
#include "camera.h"
#include "hitbox.h"
#include <stdbool.h>

#define PLAYER_SIZE 30

typedef struct {
    Coord position;
    double angle;
    Hitbox outerHitbox;
    Hitbox innerHitbox;
} Player;

void playerDraw(const Player player, bool drawHitboxes, const GDFCamera camera);
