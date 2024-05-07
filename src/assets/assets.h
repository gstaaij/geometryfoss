#pragma once
#include "raylib.h"
#include "coord.h"

typedef enum {
    ASSET_RESOLUTION_LOW = 1,
    ASSET_RESOLUTION_HD = 2,
    ASSET_RESOLUTION_UHD = 4,
} AssetResolution;

#define ASSET_RESOLUTION ASSET_RESOLUTION_UHD
#if ASSET_RESOLUTION==ASSET_RESOLUTION_UHD
    #define ASSET_RESOLUTION_STRING "-uhd"
#elif ASSET_RESOLUTION==ASSET_RESOLUTION_HD
    #define ASSET_RESOLUTION_STRING "-hd"
#else
    #define ASSET_RESOLUTION_STRING ""
#endif

const char* assetsGetAbsolutePath(const char* relativePath);

Image assetsImage(const char* relativePath);
Texture assetsTexture(const char* relativePath);

typedef struct {
    char* spriteSheet;
    Coord spriteOffset;
    Coord spriteSize;
    Rectangle textureRect;
    bool textureRotated;
} TextureMap;

typedef struct {
    char* key;
    TextureMap item;
} TextureMapItem;

bool assetsInitializeTextureMaps();
TextureMap assetsTextureMap(const char* fileName);

void assetsUnloadEverything();
