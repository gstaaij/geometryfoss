#pragma once
#include "screen.h"
#include "hitbox.h"
#include <stdlib.h>
#include <stdbool.h>

// A struct with a position, angle, scale and ID pointing to a ObjectDefenition
typedef struct {
    Coord position;
    double angle;
    double scale;
    int id;
} Object;

// Defines the type of object
typedef enum ObjectType {
    NONSOLID = 0,   // Doesn't have a hitbox
    SOLID,          // Has a hitbox that can be stepped on
    HAZARD,         // Has a hitbox that kills the player
    PORTAL,         // Has a hitbox that transforms the player into a different gamemode or gives the player a different speed
    PAD,            // Has a hitbox that launches the player into the air
    RING,           // Has a hitbox that, when someone clicks, launches the player into the air
} ObjectType;

// Defines the shape of an object
typedef enum ObjectShapeType {
    BLOCK,      // It's a square.
    SPIKE,      // It's a triangle.
} ObjectShapeType;

// Defines the shape and scale of an object
typedef struct {
    ObjectShapeType type;
    double scale;
} ObjectShape;

// Defines an object with a type, a shape, and a hitbox
typedef struct {
    ObjectType type;
    ObjectShape shape;
    Hitbox hitbox;
} ObjectDefinition;

// Draws an Object
void objectDraw(const Object object, const bool drawHitbox, const GDFCamera camera);

// An array of Object Defenitions to define all objects
static const ObjectDefinition objectDefenitions[] = {
    {0}, // ID 0 is nothing
    { // ID 1, default block
        .type = SOLID,
        .shape = {
            .type = BLOCK,
            .scale = 1,
        },
        .hitbox = {
            .shape = SQUARE,
            .offset = {0},
            .width = 30,
            .height = 30,
        },

    },
};
