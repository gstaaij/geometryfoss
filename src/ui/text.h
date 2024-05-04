#pragma once
#include <stddef.h>
#include "raylib.h"
#include "coord.h"
#include "camera.h"

char* textWrap(char* text, const size_t maxChars);

void textDrawCentered(GDFCamera camera, Font font, const char* text, Coord center, double fontSize, Color color);
