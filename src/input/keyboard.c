#include "keyboard.h"
#include <stddef.h>

#define MAX_KEYBOARD_KEYS 512

bool previousKeys[MAX_KEYBOARD_KEYS] = {0};
bool currentKeys[MAX_KEYBOARD_KEYS] = {0};

void keyboardUpdate() {
    for (size_t i = 0; i < MAX_KEYBOARD_KEYS; ++i) {
        previousKeys[i] = currentKeys[i];
        currentKeys[i] = IsKeyDown(i);
    }
}

bool keyboardMod(const bool holdShift, const bool holdControl) {
    if (!holdShift && (keyboardDown(KEY_LEFT_SHIFT) || keyboardDown(KEY_RIGHT_SHIFT))) return false;
    if (holdShift && !(keyboardDown(KEY_LEFT_SHIFT) || keyboardDown(KEY_RIGHT_SHIFT))) return false;

    if (!holdControl && (keyboardDown(KEY_LEFT_CONTROL) || keyboardDown(KEY_RIGHT_CONTROL))) return false;
    if (holdControl && !(keyboardDown(KEY_LEFT_CONTROL) || keyboardDown(KEY_RIGHT_CONTROL))) return false;

    return true;
}

bool keyboardDown(const KeyboardKey key) {
    return currentKeys[key];
}

bool keyboardDownMod(const KeyboardKey key, const bool holdShift, const bool holdControl) {
    return keyboardDown(key) && keyboardMod(holdShift, holdControl);
}

bool keyboardPressed(const KeyboardKey key) {
    return keyboardPressedMod(key, false, false);
}

bool keyboardPressedMod(const KeyboardKey key, const bool holdShift, const bool holdControl) {
    return !previousKeys[key] && currentKeys[key] && keyboardMod(holdShift, holdControl);
}

bool keyboardReleased(const KeyboardKey key) {
    return previousKeys[key] && !currentKeys[key];
}

bool keyboardReleasedMod(const KeyboardKey key, const bool holdShift, const bool holdControl) {
    return keyboardReleased(key) && keyboardMod(holdShift, holdControl);
}