#include "popup.h"
#include "nob.h"
#include "stb_ds.h"
#include "raygui.h"
#include "ui/font.h"
#include "ui/text.h"
#include "lib/easing/easing.h"
#include "input/keyboard.h"

#define POPUP_BUTTON_FONT_SIZE 12.5
#define POPUP_BUTTON_WIDTH 60.0
#define POPUP_BUTTON_HEIGHT 15.0

#define POPUP_MESSAGE_FONT_SIZE 15.0

// The width of the popup box in characters
#define POPUP_BOX_MAX_WIDTH_CHARS 40
#define POPUP_BOX_PADDING 10.0

#define POPUP_DEFAULT_COLOR (Color) { 0, 25, 49, 255 }
#ifndef DEBUG
    #define POPUP_DEFAULT_TRANSITION_TIME 0.25
#else
    #define POPUP_DEFAULT_TRANSITION_TIME 2.5
#endif


static Popup* popupQueue = NULL;
static PopupButton popupResult = POPUP_BUTTON_NONE;
static double popupTransitionProgress = 0.0;
static double popupScale = 0.0;
// We have to have one frame of debounce in case there is a button underneath the popup button
static double popupWasShown = false;

#ifdef DEBUG
    static double maxPopupScale = 0.0;
    static double maxPopupScaleTransitionProgress = 0.0;
    static double maxPopupTransitionProgress = 0.0;
    static double maxPopupTransitionProgressScale = 0.0;

    static double minPopupScale = 100.0;
    static double minPopupScaleTransitionProgress = 1.0;
    static double minPopupTransitionProgress = 1.0;
    static double minPopupTransitionProgressScale = 100.0;
#endif

long popupGetWidth(const GDFCamera uiCamera, const double fontSize, const double fontSpacing, const char* message) {
    long width = convertToScreen(4 * POPUP_BOX_PADDING, uiCamera);
    width += MeasureTextEx(fontGetSmall(), message, fontSize, fontSpacing).x;
    return width;
}

void popupNext() {
    if (arrlenu(popupQueue) > 0)
        arrdel(popupQueue, 0);
    popupTransitionProgress = 0.0;
    popupScale = 0.0;
#ifdef DEBUG
    maxPopupScale = 0.0;
    maxPopupScaleTransitionProgress = 0.0;
    maxPopupTransitionProgress = 0.0;
    maxPopupTransitionProgressScale = 0.0;

    minPopupScale = 100.0;
    minPopupScaleTransitionProgress = 1.0;
    minPopupTransitionProgress = 1.0;
    minPopupTransitionProgressScale = 100.0;
#endif
}

void popupShow(char* message) {
    message = textWrap(message, POPUP_BOX_MAX_WIDTH_CHARS);
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
    message = textWrap(message, POPUP_BOX_MAX_WIDTH_CHARS);
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
    if (!popupIsShown() || popupTransitionProgress >= 1.0)
        return;
    popupTransitionProgress += deltaTime / popupQueue[0].transitionTime;
    if (popupTransitionProgress >= 1.0)
        popupTransitionProgress = 1.0;
    popupScale = getEasingFunction(EASE_OUT_BACK)(popupTransitionProgress);
#ifdef DEBUG
    if (popupTransitionProgress > maxPopupTransitionProgress) {
        maxPopupTransitionProgress = popupTransitionProgress;
        maxPopupTransitionProgressScale = popupScale;
    }
    if (popupTransitionProgress < minPopupTransitionProgress) {
        minPopupTransitionProgress = popupTransitionProgress;
        minPopupTransitionProgressScale = popupScale;
    }

    if (popupScale > maxPopupScale) {
        maxPopupScale = popupScale;
        maxPopupScaleTransitionProgress = popupTransitionProgress;
    }
    if (popupScale < minPopupScale) {
        minPopupScale = popupScale;
        minPopupScaleTransitionProgress = popupTransitionProgress;
    }

    if (popupTransitionProgress >= 1.0) {
        TraceLog(LOG_DEBUG, "");
        TraceLog(LOG_DEBUG, "Popup at maximum scale: { transition: %lf; scale: %lf }", maxPopupScaleTransitionProgress, maxPopupScale);
        TraceLog(LOG_DEBUG, "Popup at minimum scale: { transition: %lf; scale: %lf }", minPopupScaleTransitionProgress, minPopupScale);
        TraceLog(LOG_DEBUG, "");
        TraceLog(LOG_DEBUG, "Popup at maximum transition: { transition: %lf; scale: %lf }", maxPopupTransitionProgress, maxPopupTransitionProgressScale);
        TraceLog(LOG_DEBUG, "Popup at minimum transition: { transition: %lf; scale: %lf }", minPopupTransitionProgress, minPopupTransitionProgressScale);
        TraceLog(LOG_DEBUG, "");
    }
#endif
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
    double fontSpacing = fontSize/fontGetSmall().baseSize;
    long popupWidth = popupGetWidth(uiCamera, fontSize, fontSpacing, message);
    Vector2 messageSize = MeasureTextEx(fontGetSmall(), message, (float) fontSize, (float) fontSpacing);
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

    textDrawCentered(
        uiCamera,
        fontGetSmall(), message,
        popupMessageLocation,
        POPUP_MESSAGE_FONT_SIZE * popupScale, WHITE
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
