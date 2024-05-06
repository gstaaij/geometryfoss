#pragma once
#include "raylib.h"

const char* assetsGetAbsolutePath(const char* relativePath);

Image assetsImage(const char* relativePath);
Texture assetsTexture(const char* relativePath);
void assetsUnloadEverything();
