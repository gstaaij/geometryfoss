#pragma once
#include "raylib.h"
#include "nob.h"
#include "lib/cJSON/cJSON.h"
#include "assets/assets.h"
#include "coord.h"
#include "hitbox.h"
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
    Color baseColor;
    Color detailColor;
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
    OBJSHAPE_NONE = 0,
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
    bool exists;
    ObjectType type;
    ObjectShape shape;
    Hitbox hitbox;
    Coord placeOffset;

    const char* baseTexturePath;
    const char* detailTexturePath;
    const char* glowTexturePath;
    TextureMap baseTextureMap;
    TextureMap detailTextureMap;
    TextureMap glowTextureMap;

    Color baseDefaultColor;
    Color detailDefaultColor;
} ObjectDefinition;

// Draws an Object
void objectDraw(const Object object, const bool drawGlow, const GDFCamera camera);
// Draws an Object's hitbox
void objectDrawHitbox(const Object object, const bool drawHitbox, const GDFCamera camera);

// Check if the mouse is hovering over this object
bool objectMouseOver(const Object object, const Coord clickPos);

// Serialize an Object to cJSON
cJSON* objectSerialize(const Object object);
// Deserialize an Object from cJSON
bool objectDeserialize(Object* object, const cJSON* objectJson);


typedef enum {
    OBJECT_ID_SQUARE_01 = 1,
    OBJECT_ID_SQUARE_02 = 2,
    OBJECT_ID_SQUARE_03 = 3,
    OBJECT_ID_SQUARE_04 = 4,
    OBJECT_ID_SQUARE_05 = 5,
    OBJECT_ID_SQUARE_06 = 6,
    OBJECT_ID_SQUARE_07 = 7,
    OBJECT_ID_SQUARE_08 = 83,
    /// TODO: square_09
    OBJECT_ID_SPIKE_01 = 8,
    /// TODO: This one should choose randomly between `pit_01_001.png`, `pit_02_001.png` and `pit_03_001.png` when it's placed
    OBJECT_ID_PIT_01_02_03 = 9,
} ObjectIDs;

#define TEXTURE(base, detail, glow) .baseTexturePath = (base), .detailTexturePath = (detail), .glowTexturePath = (glow)
#define DEFAULT_COLORS(base, detail) .baseDefaultColor = (base), .detailDefaultColor = (detail)
#define DEFAULT_COLOR() DEFAULT_COLORS(WHITE, BLACK)
// An array of Object Defenitions to define all objects
static const ObjectDefinition objectDefenitions[] = {
    [OBJECT_ID_SQUARE_01] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_01_001.png", NULL, "square_01_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_02] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_02_001.png", NULL, "square_02_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_03] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_03_001.png", NULL, "square_03_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_04] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_04_001.png", NULL, "square_04_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_05] = {
        .exists = true,
        .type = OBJECT_SOLID,
        // This object has no shape and no hitbox
        .shape = {
            .type = OBJSHAPE_NONE,
            // We do have to set the scale, so it doesn't disappear off of the edge of the screen too quickly
            .scale = 1,
        },
        .hitbox = {0},
        .placeOffset = {0},
        TEXTURE("square_05_001.png", NULL, NULL),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_06] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_06_001.png", NULL, "square_06_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_07] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_07_001.png", NULL, "square_07_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SQUARE_08] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("square_08_001.png", NULL, "square_08_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_SPIKE_01] = {
        .exists = true,
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
        .placeOffset = {0},
        TEXTURE("spike_01_001.png", NULL, "spike_01_glow_001.png"),
        DEFAULT_COLOR(),
    },
    [OBJECT_ID_PIT_01_02_03] = {
        .exists = true,
        .type = OBJECT_HAZARD,
        .shape = {
            .type = OBJSHAPE_SPIKE,
            .scale = 1,
        },
        .hitbox = {
            .shape = HITBOX_SQUARE,
            .offset = {0},
            .width = 9,
            // Height is slightly more than 11, but it's less than 11.5, so I don't know what the exact value should be
            .height = 11,
        },
        .placeOffset = { 0, -12.5 },
        TEXTURE("pit_01_001.png", NULL, NULL),
        DEFAULT_COLORS(BLACK, BLACK),
    },
};
#undef TEXTURE
