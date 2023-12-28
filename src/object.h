#pragma once
#include "screen.h"
#include <stdlib.h>

typedef struct {
    Coord position;
    double angle;
    double scale;
    int id;
} Object;

typedef enum ObjectType {
    NONSOLID = 0,
    SOLID,
    HAZARD,
    PORTAL,
    PAD,
    RING,
} ObjectType;

typedef enum ObjectShapeType {
    BLOCK,
    SPIKE,
} ObjectShapeType;

typedef struct {
    ObjectShapeType type;
    double scale;
} ObjectShape;

typedef enum HitboxShape {
    SQUARE,
    CIRCLE,
} HitboxShape;

typedef struct {
    HitboxShape shape;
    Coord offset;
    double width;
    double height;
} ObjectHitbox;

typedef struct {
    ObjectType type;
    ObjectShape shape;
    ObjectHitbox hitbox;
} ObjectDefinition;

void objectDraw(const Object object, const bool drawHitbox, const Coord cameraCoord, const Coord screenSizeAsCoord, const ScreenCoord screenSize);

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
