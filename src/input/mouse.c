#include <stddef.h>
#include "mouse.h"
#include "keyboard.h"

#define MAX_MOUSE_BUTTONS 8

bool previousMouseButtons[MAX_MOUSE_BUTTONS] = {0};
bool currentMouseButtons[MAX_MOUSE_BUTTONS] = {0};

Vector2 previousMousePos = {0};
Vector2 currentMousePos = {0};

void mouseUpdate() {
    for (size_t i = 0; i < MAX_MOUSE_BUTTONS; ++i) {
        previousMouseButtons[i] = currentMouseButtons[i];
        currentMouseButtons[i] = IsMouseButtonDown(i);
    }

    previousMousePos = currentMousePos;
    currentMousePos = GetMousePosition();
}


bool mouseDown(MouseButton button) {
    return currentMouseButtons[button];
}

bool mouseDownMod(MouseButton button, bool holdShift, bool holdControl) {
    return mouseDown(button) && keyboardMod(holdShift, holdControl);
}

bool mousePressed(MouseButton button) {
    return !previousMouseButtons[button] && currentMouseButtons[button];
}

bool mousePressedMod(MouseButton button, bool holdShift, bool holdControl) {
    return mousePressed(button) && keyboardMod(holdShift, holdControl);
}

bool mouseReleased(MouseButton button) {
    return previousMouseButtons[button] && !currentMouseButtons[button];
}

bool mouseReleasedMod(MouseButton button, bool holdShift, bool holdControl) {
    return mouseReleased(button) && keyboardMod(holdShift, holdControl);
}

Vector2 mouseGetDelta() {
    return (Vector2) { currentMousePos.x - previousMousePos.x, currentMousePos.y - previousMousePos.y };
}
