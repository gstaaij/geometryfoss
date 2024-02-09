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
#define POPUP_BOX_MAX_WIDTH_CHARS 40
#define POPUP_BOX_PADDING 10.0

#define POPUP_DEFAULT_COLOR (Color) { 0, 25, 49, 255 }


static Popup* popupQueue = NULL;
static PopupButton popupResult = POPUP_BUTTON_NONE;

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
    SetTextLineSpacing(convertToScreen(POPUP_BOX_PADDING / 2, uiCamera) + fontSize);

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
    Vector2 messageSize = MeasureTextEx(GetFontDefault(), message, (float) fontSize, (float) fontSpacing);
    long popupHeight = convertToScreen(POPUP_BOX_PADDING * 3 + POPUP_BUTTON_HEIGHT, uiCamera) + messageSize.y;
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

    fontSize = convertToScreen(POPUP_BUTTON_FONT_SIZE, uiCamera);

    // Set the font size
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    Coord buttonLocation = {
        popupLocation.x,
        popupLocation.y - popupGDHeight / 2 + POPUP_BOX_PADDING + POPUP_BUTTON_HEIGHT / 2,
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
