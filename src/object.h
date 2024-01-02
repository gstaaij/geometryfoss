#pragma once
#include "raylib.h"
#include "coord.h"
#include "hitbox.h"
#include "objdefspad.h"
#include <stdbool.h>

#define SOLID_OBJECT_HITBOX_COLOR   CLITERAL (Color){ 0,   0,   255, 255 }
#define HAZARD_OBJECT_HITBOX_COLOR  CLITERAL (Color){ 255, 0,   0,   255 }
#define PORTAL_OBJECT_HITBOX_COLOR  CLITERAL (Color){ 0,   255, 0,   255 }
#define PAD_OBJECT_HITBOX_COLOR     CLITERAL (Color){ 0,   255, 0,   255 }
#define RING_OBJECT_HITBOX_COLOR    CLITERAL (Color){ 0,   255, 0,   255 }

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
    PAD(6), // Until ID 7, we currently only need the default spike and the default block
    { // ID 8, default spike
        .type = HAZARD,
        .shape = {
            .type = SPIKE,
            .scale = 1,
        },
        .hitbox = {
            .shape = SQUARE,
            .offset = {0},
            .width = 6,
            .height = 12,
        }
    },
};
