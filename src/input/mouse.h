#pragma once
#include <stdbool.h>
#include "raylib.h"

void mouseUpdate();

// Returns if a given mouse button is held down. Does not care about modifier keys
bool mouseDown(MouseButton button);
// Returns if a given mouse button is held down. Can only return true if the given modifier keys are also pressed
bool mouseDownMod(MouseButton button, bool holdShift, bool holdControl);

// Returns if a given mouse button has been pressed. Does not care about modifier keys
bool mousePressed(MouseButton button);
// Returns if a given mouse button has been pressed. Can only return true if the given modifier keys are also pressed
bool mousePressedMod(MouseButton button, bool holdShift, bool holdControl);

// Returns if a given mouse button has been released. Does not care about modifier keys
bool mouseReleased(MouseButton button);
// Returns if a given mouse button has been released. Can only return true if the given modifier keys are also pressed
bool mouseReleasedMod(MouseButton button, bool holdShift, bool holdControl);

// Returns the difference between the mouse position of the current frame and the mouse position of the previous frame
Vector2 mouseGetDelta();
