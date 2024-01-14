#pragma once
#include <stdbool.h>
#include "object.h"

// Adds the object at a specific index in the objects array to the selection
void selectAddObjectIndex(Object* objects, int index, bool add);

// Adds the object at the mouse position to the selection
void selectAddObjectClicked(Object* objects, Coord clickPos, bool add);
