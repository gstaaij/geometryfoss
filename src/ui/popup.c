#include "popup.h"
#include <stddef.h>
#include "nob.h"
#include "stb_ds.h"
#include "raygui.h"
#include "input/keyboard.h"

#define POPUP_BUTTON_FONT_SIZE 12.5
#define POPUP_BUTTON_WIDTH 60.0
#define POPUP_BUTTON_HEIGHT 15.0

#define POPUP_MESSAGE_FONT_SIZE 15.0

// The width of the popup box in characters
#define POPUP_BOX_MAX_WIDTH_CHARS 40
#define POPUP_BOX_PADDING 10.0

#define POPUP_DEFAULT_COLOR (Color) { 0, 25, 49, 255 }
#define POPUP_DEFAULT_TRANSITION_TIME 0.15


static Popup* popupQueue = NULL;
static PopupButton popupResult = POPUP_BUTTON_NONE;
static double popupScale = 0.0;
// We have to have one frame of debounce in case there is a button underneath the popup button
static double popupWasShown = false;

static char* wrapString(char* string, const size_t maxChars) {
    if (maxChars <= 0)
        return string;
    
    Nob_String_View sv = nob_sv_from_cstr(string);
    Nob_String_Builder sb = {0};
    
    size_t lineLength = 0;
    while (sv.count > 0) {
        Nob_String_View subsv = nob_sv_chop_by_delim(&sv, ' ');
        bool shouldAddNewline = false;
        size_t addedLineLength = 0;
        for (size_t i = 0; i < subsv.count; ++i) {
            ++addedLineLength;
            if (subsv.data[i] == '\n') {
                addedLineLength = 0;
                lineLength = 0;
            }
            if (lineLength + addedLineLength > maxChars)
                shouldAddNewline = true;
        }
        if (shouldAddNewline) {
            nob_da_append(&sb, '\n');
            lineLength = 0;
        }
        nob_sb_append_buf(&sb, subsv.data, subsv.count);
        nob_da_append(&sb, ' ');
        lineLength += addedLineLength + 1;
    }
    nob_sb_append_null(&sb);
    return sb.items;
}

long popupGetWidth(const GDFCamera uiCamera, const double fontSize, const double fontSpacing, const char* message) {
    long width = convertToScreen(4 * POPUP_BOX_PADDING, uiCamera);
    width += MeasureTextEx(GetFontDefault(), message, fontSize, fontSpacing).x;
    return width;
}

void popupNext() {
    if (arrlenu(popupQueue) > 0)
        arrdel(popupQueue, 0);
    popupScale = 0.0;
}

void popupShow(char* message) {
    message = wrapString(message, POPUP_BOX_MAX_WIDTH_CHARS);
    popupShowEx((Popup) {
        .message = message,
        .buttonOneText = "OK",
        .twoButtons = false,
        .buttonTwoText = NULL,
        .color = POPUP_DEFAULT_COLOR,
        .transitionTime = POPUP_DEFAULT_TRANSITION_TIME,
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
        .transitionTime = POPUP_DEFAULT_TRANSITION_TIME,
    });
}

void popupShowEx(Popup popup) {
    arrput(popupQueue, popup);
    if (!popupIsShown()) popupNext();
}

PopupButton popupGetChoiceResult() {
    PopupButton result = popupResult;
    popupResult = POPUP_BUTTON_NONE;
    return result;
}

bool popupIsShown() {
    return popupWasShown || arrlenu(popupQueue) > 0;
}

void popupUpdate(const double deltaTime) {
    if (!popupIsShown() || popupScale >= 1.0)
        return;
    popupScale += deltaTime / popupQueue[0].transitionTime;
    if (popupScale >= 1.0)
        popupScale = 1.0;
}

void popupUpdateUI(const GDFCamera uiCamera) {
    popupWasShown = arrlenu(popupQueue) > 0;
    if (!popupIsShown())
        return;

    // Lock the GUI while the popup is still small, to eliminate
    // the chances of the user clicking on a button and
    // immediately also clicking on a popup button that appears
    if (popupScale < 0.25)
        GuiLock();

    Popup currentPopup = popupQueue[0];
    
    double fontSize = convertToScreen(POPUP_MESSAGE_FONT_SIZE * popupScale, uiCamera);
    SetTextLineSpacing(convertToScreen(POPUP_BOX_PADDING / 2 * popupScale, uiCamera) + fontSize);

    const char* message = currentPopup.message;

    Coord popupLocation = { 0, 0 };
    Coord popupMessageLocation = {
        popupLocation.x,
        popupLocation.y + (POPUP_BOX_PADDING / 2 + POPUP_BUTTON_HEIGHT / 2) * popupScale,
    };
    ScreenCoord popupScreenLocation = getScreenCoord(popupLocation, uiCamera);
    ScreenCoord popupMessageScreenLocation = getScreenCoord(popupMessageLocation, uiCamera);
    double fontSpacing = fontSize/GetFontDefault().baseSize;
    long popupWidth = popupGetWidth(uiCamera, fontSize, fontSpacing, message);
    Vector2 messageSize = MeasureTextEx(GetFontDefault(), message, (float) fontSize, (float) fontSpacing);
    long popupHeight = convertToScreen((POPUP_BOX_PADDING * 3 + POPUP_BUTTON_HEIGHT) * popupScale, uiCamera) + messageSize.y;
    double popupGDHeight = convertToGD(popupHeight, uiCamera);

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

#ifdef DEBUG
    DrawRectanglePro(
        (Rectangle) {
            popupMessageScreenLocation.x, popupMessageScreenLocation.y,
            messageSize.x, messageSize.y,
        },
        (Vector2) { messageSize.x / 2, messageSize.y / 2 },
        0.0f,
        RED
    );
#endif

    DrawTextPro(
        GetFontDefault(), message,
        (Vector2) { popupMessageScreenLocation.x, popupMessageScreenLocation.y },
        (Vector2) { messageSize.x / 2, messageSize.y / 2 },
        0.0f,
        (float) fontSize, (float) fontSpacing,
        LIGHTGRAY
    );

    fontSize = convertToScreen(POPUP_BUTTON_FONT_SIZE * popupScale, uiCamera);

    // Set the font size
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    Coord buttonLocation = {
        popupLocation.x,
        popupLocation.y - popupGDHeight / 2 + (POPUP_BOX_PADDING + POPUP_BUTTON_HEIGHT / 2) * popupScale,
    };
    if (currentPopup.twoButtons) {
        buttonLocation.x -= (POPUP_BOX_PADDING / 2 + POPUP_BUTTON_WIDTH / 2) * popupScale;
    }
    ScreenCoord buttonScreenLocation = getScreenCoord(buttonLocation, uiCamera);
    long buttonWidth = convertToScreen(POPUP_BUTTON_WIDTH * popupScale, uiCamera);
    long buttonHeight = convertToScreen(POPUP_BUTTON_HEIGHT * popupScale, uiCamera);

    bool clickedButtonOne = GuiButton(
        (Rectangle) {
            buttonScreenLocation.x - buttonWidth / 2, buttonScreenLocation.y - buttonHeight / 2,
            buttonWidth, buttonHeight,
        },
        currentPopup.buttonOneText
    );

    if (!currentPopup.twoButtons && keyboardPressed(KEY_ENTER)) {
        clickedButtonOne = true;
    }

    bool clickedButtonTwo = false;
    if (currentPopup.twoButtons) {
        buttonLocation.x += (POPUP_BOX_PADDING + POPUP_BUTTON_WIDTH) * popupScale;
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
        popupNext();
    } else if (clickedButtonTwo) {
        popupResult = POPUP_BUTTON_TWO;
        popupNext();
    }

    GuiUnlock();
}
