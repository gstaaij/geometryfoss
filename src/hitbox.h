#pragma once
#include "raylib.h"
#include "coord.h"

// Defines the shape of a hitbox
typedef enum HitboxShape {
    HITBOX_SQUARE,
    HITBOX_CIRCLE,
} HitboxShape;

// Defines a hitbox with a shape, an offset, a width and a height
typedef struct {
    HitboxShape shape;
    Coord offset;
    double width;
    double height;
} Hitbox;

// Draw a hitbox
void hitboxDraw(const Hitbox hitbox, const Coord parentPosition, const double scale, const Color hitboxColor, const GDFCamera camera);

// Check if a hitbox collides with another hitbox
bool hitboxCollides(const Hitbox hitbox1, const Coord parent1Position, const Hitbox hitbox2, const Coord parent2Position);
// Check if a square hitbox collides with another square hitbox, but only check the x axis
bool hitboxSquareCollidesOnlyX(const Hitbox hitbox1, const Coord parent1Position, const Hitbox hitbox2, const Coord parent2Position);
// Check if a square hitbox collides with another square hitbox, but only check the y axis
bool hitboxSquareCollidesOnlyY(const Hitbox hitbox1, const Coord parent1Position, const Hitbox hitbox2, const Coord parent2Position);
