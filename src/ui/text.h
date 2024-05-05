#pragma once
#include <stddef.h>
#include "raylib.h"
#include "coord.h"
#include "camera.h"

char* textWrap(char* text, const size_t maxChars);

void textDraw(Font font, const char* text, int posX, int posY, int fontSize, Color color);
void textDrawCentered(GDFCamera camera, Font font, const char* text, Coord center, double fontSize, Color color);
