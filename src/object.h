#pragma once
#include "raylib.h"
#include "nob.h"
#include "cJSON/cJSON.h"
#include "coord.h"
#include "hitbox.h"
#include "objdefsutil.h"
#include <stdbool.h>

#define OBJECT_SOLID_HITBOX_COLOR   CLITERAL (Color){ 0,   0,   255, 255 }
#define OBJECT_HAZARD_HITBOX_COLOR  CLITERAL (Color){ 255, 0,   0,   255 }
#define OBJECT_PORTAL_HITBOX_COLOR  CLITERAL (Color){ 0,   255, 0,   255 }
#define OBJECT_PAD_HITBOX_COLOR     CLITERAL (Color){ 0,   255, 0,   255 }
#define OBJECT_RING_HITBOX_COLOR    CLITERAL (Color){ 0,   255, 0,   255 }

// A struct with a position, angle, scale and ID pointing to a ObjectDefenition
typedef struct {
    Coord position;
    double angle;
    double scale;
    bool selected;
    int id;
} Object;

// Defines the type of object
typedef enum ObjectType {
    OBJECT_NONSOLID = 0,    // Doesn't have a hitbox
    OBJECT_SOLID,           // Has a hitbox that can be stepped on
    OBJECT_HAZARD,          // Has a hitbox that kills the player
    OBJECT_PORTAL,          // Has a hitbox that transforms the player into a different gamemode or gives the player a different speed
    OBJECT_PAD,             // Has a hitbox that launches the player into the air
    OBJECT_RING,            // Has a hitbox that, when someone clicks, launches the player into the air
} ObjectType;

// Defines the shape of an object
typedef enum ObjectShapeType {
    OBJSHAPE_BLOCK,      // It's a square.
    OBJSHAPE_SPIKE,      // It's a triangle.
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
    bool exists;
} ObjectDefinition;

// Draws an Object
void objectDraw(const Object object, const GDFCamera camera);
// Draws an Object's hitbox
void objectDrawHitbox(const Object object, const bool drawHitbox, const GDFCamera camera);

// Check if the mouse is hovering over this object
bool objectMouseOver(const Object object, const Coord clickPos);

// Serialize an Object to a JSON string builder
Nob_String_Builder objectSerialize(const Object object, const int tabSize);
// Deserialize an Object from cJSON
bool objectDeserialize(Object* object, const cJSON* objectJson);

// An array of Object Defenitions to define all objects
static const ObjectDefinition objectDefenitions[] = {
    {0}, // ID 0 is nothing
    { // ID 1, default block
        .type = OBJECT_SOLID,
        .shape = {
            .type = OBJSHAPE_BLOCK,
            .scale = 1,
        },
        .hitbox = {
            .shape = HITBOX_SQUARE,
            .offset = {0},
            .width = 30,
            .height = 30,
        },
        .exists = true,
    },
    PAD(6), // Until ID 7, we currently only need the default spike and the default block
    { // ID 8, default spike
        .type = OBJECT_HAZARD,
        .shape = {
            .type = OBJSHAPE_SPIKE,
            .scale = 1,
        },
        .hitbox = {
            .shape = HITBOX_SQUARE,
            .offset = {0},
            .width = 6,
            .height = 12,
        },
        .exists = true,
    },
};
