#pragma once
#include <stdbool.h>
#include "raylib.h"
#include "camera.h"

typedef struct {
    // The message of the popup
    const char* message;
    // The text of button one
    const char* buttonOneText;
    // If true, the popup has two buttons, if not, it has one
    bool twoButtons;
    // The text of button 
    const char* buttonTwoText;
    // The color of the popup
    Color color;
} Popup;

typedef enum {
    POPUP_BUTTON_NONE = -1,

    POPUP_BUTTON_ONE,
    POPUP_BUTTON_TWO,
} PopupButton;

void popupShow(char* message);

void popupShowChoice(char* message, const char* buttonOneText, const char* buttonTwoText);

void popupShowEx(Popup popup);

PopupButton popupGetChoiceResult();

bool popupIsShown();

void popupUpdate(const double deltaTime);

void popupUpdateUI(const GDFCamera uiCamera);
