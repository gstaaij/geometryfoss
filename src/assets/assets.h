#pragma once
#include "raylib.h"
#include "coord.h"

const char* assetsGetAbsolutePath(const char* relativePath);

Image assetsImage(const char* relativePath);
Texture assetsTexture(const char* relativePath);

typedef struct {
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
