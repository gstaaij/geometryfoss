#include "popup.h"
#include <stddef.h>
#include "nob.h"
#include "stb_ds.h"
#include "raygui.h"

#define POPUP_BUTTON_FONT_SIZE 12.5
#define POPUP_BUTTON_WIDTH 60.0
#define POPUP_BUTTON_HEIGHT 15.0

#define POPUP_MESSAGE_FONT_SIZE 15.0

// The width of the popup box in characters
#define POPUP_BOX_MAX_WIDTH_CHARS 40.0
#define POPUP_BOX_PADDING 10.0

#define POPUP_DEFAULT_COLOR (Color) { 0, 25, 49, 255 }


static Popup* popupQueue = NULL;
static PopupButton popupResult = POPUP_BUTTON_NONE;

static char* wrapString(char* string, const int maxChars) {
    if (maxChars <= 0)
        return string;

    size_t len = strlen(string);
    Nob_String_Builder sb = {0};
    for (size_t i = 0; i < len; i += maxChars) {
        int count = len - i;
        if (count > maxChars)
            count = maxChars;
        nob_sb_append_buf(&sb, string + i, count);
        if (i + maxChars < len) {
            nob_da_append(&sb, '\n');
        }
    }
    nob_sb_append_null(&sb);
    return sb.items;
}

long popupGetWidth(const GDFCamera uiCamera, const int fontSize, const char* message) {
    long width = convertToScreen(4 * POPUP_BOX_PADDING, uiCamera);
    width += MeasureText(message, fontSize);
    return width;
}

void popupShow(char* message) {
    message = wrapString(message, POPUP_BOX_MAX_WIDTH_CHARS);
    popupShowEx((Popup) {
        .message = message,
        .buttonOneText = "OK",
        .twoButtons = false,
        .buttonTwoText = NULL,
        .color = POPUP_DEFAULT_COLOR,
    });
}

void popupShowChoice(char* message, const char* buttonOneText, const char* buttonTwoText) {
    message = wrapString(message, POPUP_BOX_MAX_WIDTH_CHARS);
    popupShowEx((Popup) {
        .message = message,
        .buttonOneText = buttonOneText,
        .twoButtons = true,
        .buttonTwoText = buttonTwoText,
        .color = POPUP_DEFAULT_COLOR,
    });
}

void popupShowEx(Popup popup) {
    arrput(popupQueue, popup);
}

PopupButton popupGetChoiceResult() {
    PopupButton result = popupResult;
    popupResult = POPUP_BUTTON_NONE;
    return result;
}

bool popupIsShown() {
    return arrlen(popupQueue) > 0;
}

void popupUpdate(const double deltaTime) {
    (void) deltaTime;
    /// TODO: start the popup small and grow
}

void popupUpdateUI(const GDFCamera uiCamera) {
    if (!popupIsShown())
        return;

    Popup currentPopup = popupQueue[0];
    
    long fontSize = convertToScreen(POPUP_MESSAGE_FONT_SIZE, uiCamera);

    const char* message = currentPopup.message;

    Coord popupLocation = { 0, 0 };
    Coord popupMessageLocation = {
        popupLocation.x,
        popupLocation.y + POPUP_BOX_PADDING / 2 + POPUP_BUTTON_HEIGHT / 2,
    };
    ScreenCoord popupScreenLocation = getScreenCoord(popupLocation, uiCamera);
    ScreenCoord popupMessageScreenLocation = getScreenCoord(popupMessageLocation, uiCamera);
    long fontSpacing = fontSize/GetFontDefault().baseSize;
    long popupWidth = popupGetWidth(uiCamera, fontSize, message);
    Vector2 messageSize = MeasureTextEx(GetFontDefault(), message, fontSize, fontSpacing);
    long popupHeight = convertToScreen(POPUP_BOX_PADDING * 3 + POPUP_BUTTON_HEIGHT, uiCamera) + messageSize.y;

    DrawRectangle(
        popupScreenLocation.x - popupWidth / 2, popupScreenLocation.y - popupHeight / 2,
        popupWidth, popupHeight,
        currentPopup.color
    );

    DrawRectangleLinesEx(
        (Rectangle) {
            popupScreenLocation.x - popupWidth / 2, popupScreenLocation.y - popupHeight / 2,
            popupWidth, popupHeight,
        },
        convertToScreen(1.0, uiCamera),
        GRAY
    );

    DrawTextPro(
        GetFontDefault(), message,
        (Vector2) { popupMessageScreenLocation.x, popupMessageScreenLocation.y },
        (Vector2) { messageSize.x / 2, messageSize.y / 2 },
        0.0f,
        (float) fontSize, (float) fontSpacing,
        LIGHTGRAY
    );

    fontSize = convertToScreen(POPUP_BUTTON_FONT_SIZE, uiCamera);

    // Set the font size
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    Coord buttonLocation = {
        popupLocation.x,
        popupLocation.y - POPUP_BOX_PADDING / 2 - POPUP_BUTTON_HEIGHT / 2,
    };
    if (currentPopup.twoButtons) {
        buttonLocation.x -= POPUP_BOX_PADDING / 2 + POPUP_BUTTON_WIDTH / 2;
    }
    ScreenCoord buttonScreenLocation = getScreenCoord(buttonLocation, uiCamera);
    long buttonWidth = convertToScreen(POPUP_BUTTON_WIDTH, uiCamera);
    long buttonHeight = convertToScreen(POPUP_BUTTON_HEIGHT, uiCamera);

    bool clickedButtonOne = GuiButton(
        (Rectangle) {
            buttonScreenLocation.x - buttonWidth / 2, buttonScreenLocation.y - buttonHeight / 2,
            buttonWidth, buttonHeight,
        },
        currentPopup.buttonOneText
    );

    bool clickedButtonTwo = false;
    if (currentPopup.twoButtons) {
        buttonLocation.x += POPUP_BOX_PADDING + POPUP_BUTTON_WIDTH;
        buttonScreenLocation = getScreenCoord(buttonLocation, uiCamera);
        clickedButtonTwo = GuiButton(
            (Rectangle) {
                buttonScreenLocation.x - buttonWidth / 2, buttonScreenLocation.y - buttonHeight / 2,
                buttonWidth, buttonHeight,
            },
            currentPopup.buttonTwoText
        );
    }

    if (clickedButtonOne) {
        popupResult = POPUP_BUTTON_ONE;
        arrdel(popupQueue, 0);
    } else if (clickedButtonTwo) {
        popupResult = POPUP_BUTTON_TWO;
        arrdel(popupQueue, 0);
    }
}
