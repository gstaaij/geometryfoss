#include "keyboard.h"
#include "raylib.h"

int currentKey = 0;
int previousKey = 0;

// This is needed because raylib doesn't like the seperate fps and tps
bool keyPressed(int key) {
    return currentKey == key && previousKey != key;
}

void keyboardUpdate() {
    previousKey = currentKey;
    currentKey = GetKeyPressed();
}