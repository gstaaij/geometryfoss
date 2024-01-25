#pragma once
#include <stdbool.h>
#include "raylib.h"

void keyboardUpdate();


// Returns if the given modifiers are held down
bool keyboardMod(const bool holdShift, const bool holdControl);

// Returns if a given key is held down. Does not care about modifier keys
bool keyboardDown(const KeyboardKey key);
// Returns if a given key is held down. Can only return true if the given modifier keys are also held down
bool keyboardDownMod(const KeyboardKey key, const bool holdShift, const bool holdControl);

// Returns if a given key has been pressed. Can only return true if no modifier keys are held down
bool keyboardPressed(const KeyboardKey key);
// Returns if a given key has been pressed. Can only return true if the given modifier keys are also held down
bool keyboardPressedMod(const KeyboardKey key, const bool holdShift, const bool holdControl);

// Returns if a given key has been pressed. Does not care about modifier keys
bool keyboardReleased(const KeyboardKey key);
// Returns if a given key has been released. Can only return true if the given modifier keys are also held down
bool keyboardReleasedMod(const KeyboardKey key, const bool holdShift, const bool holdControl);

/// TODO: also add keyboardGetCharPressed
