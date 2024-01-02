#pragma once
#include "raylib.h"
#include "coord.h"

// Defines the shape of a hitbox
typedef enum HitboxShape {
    SQUARE,
    CIRCLE,
} HitboxShape;

// Defines a hitbox with a shape, an offset, a width and a height
typedef struct {
    HitboxShape shape;
    Coord offset;
    double width;
    double height;
} Hitbox;

void hitboxDraw(const Hitbox hitbox, const Coord parentPosition, const double scale, const Color hitboxColor, const GDFCamera camera);

bool hitboxCollides(const Hitbox hitbox1, const Coord parent1Position, const Hitbox hitbox2, const Coord parent2Position);
