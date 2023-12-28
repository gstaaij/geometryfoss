#pragma once
#include "screen.h"

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
