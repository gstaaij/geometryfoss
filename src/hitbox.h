#pragma once
#include "raylib.h"
#include "coord.h"

// Defines the shape of a hitbox
typedef enum HitboxShape {
    HITBOX_SQUARE,
    HITBOX_CIRCLE,
} HitboxShape;

typedef enum HitboxAngle {
    HITBOX_ROTATION_MODE_90DEG,
    HITBOX_ROTATION_MODE_NONE,
    // NOT IMPLEMENTED YET
    HITBOX_ROTATION_MODE_FULL,
} HitboxRotationMode;

// Defines a hitbox with a shape, an offset, a width and a height
/// TODO: Make two different hitbox structure, one for the defenitions, one for the objects
typedef struct {
    HitboxShape shape;
    Coord offset;
    double width;
    double height;
    HitboxRotationMode rotationMode;
} Hitbox;

// Draw a hitbox
void hitboxDraw(const Hitbox hitbox, const Coord parentPosition, const double scale, const int angle, const Color hitboxColor, const GDFCamera camera);

// Set the angle of a hitbox
int hitboxGetRealAngle(const Hitbox hitbox, const int angle);

// Check if a hitbox collides with another hitbox
bool hitboxCollides(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle);
// Check if a square hitbox collides with another square hitbox, but only check the x axis
bool hitboxSquareCollidesOnlyX(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle);
// Check if a square hitbox collides with another square hitbox, but only check the y axis
bool hitboxSquareCollidesOnlyY(const Hitbox hitbox1, const Coord parent1Position, const int hitbox1Angle, const Hitbox hitbox2, const Coord parent2Position, const int hitbox2Angle);
