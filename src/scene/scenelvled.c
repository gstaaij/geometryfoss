#include "scenelvled.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "stb_ds.h"
#include "raygui.h"
#include "raymath.h"
#include "lib/cJSON/cJSON.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "grid.h"
#include "ground.h"
#include "select.h"
#include "camera.h"
#include "serialize.h"
#include "level/level.h"
#include "ui/popup.h"

#define MODE_BUTTON_OFFSET 5.0
#define MOUSE_DEAD_ZONE_UPPER_Y 90
#define MODE_BUTTON_WIDTH 75.0
#define MODE_BUTTON_HEIGHT ((MOUSE_DEAD_ZONE_UPPER_Y - MODE_BUTTON_OFFSET*4) / 3)
#define MODE_BUTTON_FONT_SIZE 18.0

#define BUTTON_GRID_COLUMNS 6.0
#define BUTTON_GRID_ROWS 2.0

#define BUTTON_GRID_OFFSET 5.0
#define BUTTON_GRID_HEIGHT (MOUSE_DEAD_ZONE_UPPER_Y - 2.0 * BUTTON_GRID_OFFSET)
#define BUTTON_GRID_BUTTON_SIZE (BUTTON_GRID_HEIGHT / BUTTON_GRID_ROWS - (BUTTON_GRID_ROWS * BUTTON_GRID_OFFSET) / 4)
#define BUTTON_GRID_WIDTH (BUTTON_GRID_COLUMNS * BUTTON_GRID_BUTTON_SIZE + (BUTTON_GRID_COLUMNS - 1) * BUTTON_GRID_OFFSET)

#define DESELECTED_BUTTON_ALPHA 0.8

#define POPUP_MESSAGE_COULDNT_SAVE "The level couldn't be saved"
#define POPUP_MESSAGE_COULDNT_LOAD "The level couldn't be loaded"

#define POPUP_MESSAGE_NOT_IMPLEMENTED "This feature isn't implemented yet :)"

// Pause menu macros
#define PAUSE_MENU_BUTTON_WIDTH 200.0
#define PAUSE_MENU_BUTTON_HEIGHT 20.0
#define PAUSE_MENU_BUTTON_OFFSET 5.0
typedef enum {
    PAUSE_MENU_BUTTON_RESUME = 0,
    PAUSE_MENU_BUTTON_SAVE,
    PAUSE_MENU_BUTTON_EXIT,
    PAUSE_MENU_BUTTON_SAVE_AND_EXIT,
    PAUSE_MENU_BUTTON_SAVE_AND_PLAY,

    PAUSE_MENU_BUTTON_COUNT,
} PauseMenuButton;
static const char* pauseMenuButtonTexts[] = {
    [PAUSE_MENU_BUTTON_RESUME] = "Resume",
    [PAUSE_MENU_BUTTON_SAVE] = "Save",
    [PAUSE_MENU_BUTTON_EXIT] = "Exit",
    [PAUSE_MENU_BUTTON_SAVE_AND_EXIT] = "Save & Exit",
    [PAUSE_MENU_BUTTON_SAVE_AND_PLAY] = "Save & Play",

    [PAUSE_MENU_BUTTON_COUNT] = "INVALID BUTTON",
};

// Popup stuff
typedef enum {
    POPUP_QUESTION_NONE = -1,

    POPUP_QUESTION_SAVE,
    POPUP_QUESTION_LOAD,
} PopupQuestion;
PopupQuestion popupQuestion = POPUP_QUESTION_NONE;

SceneLevelEditor* scenelvledCreate() {
    SceneLevelEditor* scenelvled = (SceneLevelEditor*) malloc(sizeof(SceneLevelEditor));
    assert(scenelvled != NULL && "You don't have enough RAM");
    memset(scenelvled, 0, sizeof(SceneLevelEditor));

    cameraRecalculateScreenSize(&scenelvled->camera);
    scenelvled->camera.position.x = scenelvled->camera.screenSizeAsCoord.x / 2 - 110;
    scenelvled->camera.position.y = scenelvled->camera.screenSizeAsCoord.y / 2 - 20;

    scenelvled->uiCamera.position = (Coord){0};

    scenelvled->objects = NULL;

    scenelvled->uiMode = EDITOR_UI_MODE_BUILD;

    scenelvled->levelSettings.backgroundColor = GetColor(0x287dffff);
    scenelvled->levelSettings.groundColor = GetColor(0x0066ffff);

    // Build Mode

    scenelvled->blockBuildId = 1;


    // Load the level
    if (!levelLoadFromFile("level.json", &scenelvled->levelSettings, &scenelvled->objects))
        TraceLog(LOG_WARNING, "Couldn't load save! This is likely due to the save not existing yet");

    return scenelvled;
}
void scenelvledDestroy(SceneLevelEditor* scenelvled) {
    free(scenelvled);
}


// Some global variables needed for the update loop

bool isDragging;
bool isValidClick;
double holdTime;
int startMouseX;
int startMouseY;

void scenelvledUpdate(SceneLevelEditor* scenelvled, SceneState* sceneState, double deltaTime) {
    (void) sceneState;

    popupUpdate(deltaTime);
    if (popupIsShown())
        return;

    PopupButton popupResult = popupGetChoiceResult();
    if (popupResult != POPUP_BUTTON_NONE) {
        switch (popupQuestion) {
            case POPUP_QUESTION_NONE: {} break;
            case POPUP_QUESTION_SAVE: {
                bool shouldSave = popupResult == POPUP_BUTTON_ONE;
                if (shouldSave && !levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                }
            } break;
            case POPUP_QUESTION_LOAD: {
                bool shouldLoad = popupResult == POPUP_BUTTON_ONE;
                if (shouldLoad && !levelLoadFromFile("level.json", &scenelvled->levelSettings, &scenelvled->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't load save!");
                    popupShow(POPUP_MESSAGE_COULDNT_LOAD);
                }
            } break;
        }
        popupQuestion = POPUP_QUESTION_NONE;
    }

    if (scenelvled->isPaused || scenelvled->clickedButton) {
        /// TODO: fix the bug where you place something when clicking the pause button
        scenelvled->clickedButton = false;
        return;
    }
    
    if (mousePressed(MOUSE_BUTTON_LEFT)) {
        startMouseX = GetMouseX();
        startMouseY = GetMouseY();
        holdTime = 0.0;

        isValidClick = convertToGD(scenelvled->uiCamera.screenSize.y - startMouseY, scenelvled->uiCamera) > MOUSE_DEAD_ZONE_UPPER_Y;
    }

    if (isValidClick) {
        if (isDragging) {
            holdTime += deltaTime;
            
            Vector2 mouseDelta = mouseGetDelta();
            double deltaXCoord = convertToGD(mouseDelta.x, scenelvled->camera);
            double deltaYCoord = convertToGD(mouseDelta.y, scenelvled->camera);

            scenelvled->camera.position.x -= deltaXCoord;
            scenelvled->camera.position.y += deltaYCoord;
        } else if (mouseReleased(MOUSE_BUTTON_LEFT) && !scenelvled->clickedButton) {
            
            ScreenCoord clickScreenCoord = {
                .x = GetMouseX(),
                .y = GetMouseY(),
            };
            Coord clickPos = getGDCoord(clickScreenCoord, scenelvled->camera);
            switch (scenelvled->uiMode) {
                case EDITOR_UI_MODE_BUILD: {
                    clickPos.x = floor(clickPos.x / 30) * 30 + 15;
                    clickPos.y = floor(clickPos.y / 30) * 30 + 15;
                    Object newObject = {
                        .id = scenelvled->blockBuildId,
                        .position = clickPos,
                        .angle = 0,
                        .scale = 1.0,
                    };
                    arrput(scenelvled->objects, newObject);
                    selectAddObjectIndex(scenelvled->objects, arrlen(scenelvled->objects) - 1, false);
                } break;

                case EDITOR_UI_MODE_EDIT: {
                    selectAddObjectClicked(scenelvled->objects, clickPos, IsKeyDown(KEY_LEFT_SHIFT));
                } break;

                case EDITOR_UI_MODE_DELETE: {
                    for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
                        if (objectMouseOver(scenelvled->objects[i], clickPos)) {
                            arrdel(scenelvled->objects, i);
                            break;
                        }
                    }
                } break;
            }

        } else if (mouseDown(MOUSE_BUTTON_LEFT)) {
            holdTime += deltaTime;
            int deltaX = GetMouseX() - startMouseX;
            int deltaY = GetMouseY() - startMouseY;
            int mouseDeltaLengthSq = deltaX*deltaX + deltaY*deltaY;
            
            int maxMouseDelta = convertToScreen(30, scenelvled->uiCamera);

            if (mouseDeltaLengthSq > maxMouseDelta*maxMouseDelta) {
                isDragging = true;
            }
        }
    }

    if (mouseReleased(MOUSE_BUTTON_LEFT)) {
        startMouseX = startMouseY = -1;
        isDragging = false;
    }


    bool shiftDown = keyboardDown(KEY_LEFT_SHIFT);
    // Moving objects around
    bool keyPressedW = keyboardPressed(KEY_W) || keyboardPressedMod(KEY_W, true, false);
    bool keyPressedS = keyboardPressed(KEY_S) || keyboardPressedMod(KEY_S, true, false);
    bool keyPressedA = keyboardPressed(KEY_A) || keyboardPressedMod(KEY_A, true, false);
    bool keyPressedD = keyboardPressed(KEY_D) || keyboardPressedMod(KEY_D, true, false);
    // Rotating objects
    bool keyPressedQ = keyboardPressed(KEY_Q);
    bool keyPressedE = keyboardPressed(KEY_E);
    // Deleting objects
    bool keyPressedDel = keyboardPressed(KEY_DELETE);

    bool usefulKeyPressed = keyPressedW || keyPressedS || keyPressedA || keyPressedD || keyPressedQ || keyPressedE || keyPressedDel;
    // Move or delete selected blocks if the correct key is pressed
    if (usefulKeyPressed) {
        // The middle point of the selection
        Coord selectedObjectRotateAroundPosition = {0};
        if (keyPressedQ || keyPressedE) {
            double minX =  INFINITY;
            double maxX = -INFINITY;
            double minY =  INFINITY;
            double maxY = -INFINITY;
            for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
                Object* object = &scenelvled->objects[i];
                if (object->selected) {
                    double x = object->position.x;
                    double y = object->position.y;
                    if (x < minX)
                        minX = x;
                    if (x > maxX)
                        maxX = x;
                    
                    if (y < minY)
                        minY = y;
                    if (y > maxY)
                        maxY = y;
                }
            }
            selectedObjectRotateAroundPosition = (Coord) {
                .x = (minX + maxX) / 2,
                .y = (minY + maxY) / 2,
            };
            TraceLog(LOG_DEBUG, "Selected object rotation position: {%lf, %lf}", selectedObjectRotateAroundPosition.x, selectedObjectRotateAroundPosition.y);
        }
        for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
            Object* object = &scenelvled->objects[i];
            if (object->selected) {
                if (keyPressedW) {
                    object->position.y += 2 + !shiftDown * 28;
                }
                if (keyPressedS) {
                    object->position.y -= 2 + !shiftDown * 28;
                }
                if (keyPressedA) {
                    object->position.x -= 2 + !shiftDown * 28;
                }
                if (keyPressedD) {
                    object->position.x += 2 + !shiftDown * 28;
                }
                if (keyPressedQ) {
                    Vector2 relativeObjectPosition = {
                        .x = object->position.x - selectedObjectRotateAroundPosition.x,
                        .y = object->position.y - selectedObjectRotateAroundPosition.y,
                    };
                    // Raymath works with radians, but the rest of raylib works with degrees. Makes sense.
                    Vector2 newRelativeObjectPosition = Vector2Rotate(relativeObjectPosition, 0.5*PI);

                    object->position.x = newRelativeObjectPosition.x + selectedObjectRotateAroundPosition.x;
                    object->position.y = newRelativeObjectPosition.y + selectedObjectRotateAroundPosition.y;

                    object->angle -= 90;
                    if (object->angle < 0) object->angle += 360;
                }
                if (keyPressedE) {
                    Vector2 relativeObjectPosition = {
                        .x = object->position.x - selectedObjectRotateAroundPosition.x,
                        .y = object->position.y - selectedObjectRotateAroundPosition.y,
                    };
                    // Also, a positive angle rotates counterclockwise in raymath and clockwise in other raylib functions (like DrawRectanglePro, see player/player.c)
                    Vector2 newRelativeObjectPosition = Vector2Rotate(relativeObjectPosition, -0.5*PI);

                    object->position.x = newRelativeObjectPosition.x + selectedObjectRotateAroundPosition.x;
                    object->position.y = newRelativeObjectPosition.y + selectedObjectRotateAroundPosition.y;

                    object->angle += 90;
                    if (object->angle > 360) object->angle -= 360;
                }
                if (keyPressedDel) {
                    arrdel(scenelvled->objects, i);
                }
            }
        }
    }

    // Zooming in
    if (keyboardPressedMod(KEY_EQUAL, false, true)) {
        cameraSetZoomLevel(&scenelvled->camera, scenelvled->camera.zoomLevel + 0.1);
    }
    // Zooming out
    if (keyboardPressedMod(KEY_MINUS, false, true)) {
        cameraSetZoomLevel(&scenelvled->camera, scenelvled->camera.zoomLevel - 0.1);
    }
    // Zooming with the mouse
    if (GetMouseWheelMove() != 0 && keyboardDown(KEY_LEFT_CONTROL)) {
        cameraSetZoomLevel(&scenelvled->camera, scenelvled->camera.zoomLevel + (GetMouseWheelMove() > 0 ? 0.1 : -0.1));
    }

    // Save the level
    if (keyboardPressedMod(KEY_S, false, true)) {
        popupQuestion = POPUP_QUESTION_SAVE;
        popupShowChoice("Are you sure you want to save the level?", "Yes", "No");
    }

    // Load the level
    if (keyboardPressedMod(KEY_L, false, true)) {
        popupQuestion = POPUP_QUESTION_LOAD;
        popupShowChoice("Are you sure you want to load the level?\nAll your unsaved work will be lost!", "Yes", "No");
    }

    switch (scenelvled->uiMode) {
        case EDITOR_UI_MODE_BUILD: {} break;
        case EDITOR_UI_MODE_EDIT: {} break;
        case EDITOR_UI_MODE_DELETE: {} break;
    }
}

bool shouldLockUI(SceneLevelEditor* scenelvled) {
    return scenelvled->isPaused || popupIsShown();
}

void scenelvledUpdateUI(SceneLevelEditor* scenelvled, SceneState* sceneState) {
    cameraRecalculateScreenSize(&scenelvled->uiCamera);

    if (shouldLockUI(scenelvled))
        GuiLock();

    // Convert some sizes from GD coordinates to screen coordinates

    long buttonOffset = convertToScreen(MODE_BUTTON_OFFSET, scenelvled->uiCamera);
    long buttonWidth = convertToScreen(MODE_BUTTON_WIDTH, scenelvled->uiCamera);
    long buttonHeight = convertToScreen(MODE_BUTTON_HEIGHT, scenelvled->uiCamera);
    long fontSize = convertToScreen(MODE_BUTTON_FONT_SIZE, scenelvled->uiCamera);
    long upperY = convertToScreen(MOUSE_DEAD_ZONE_UPPER_Y, scenelvled->uiCamera);

    // Draw the bottom button pane
    DrawRectangle(0, scenelvled->uiCamera.screenSize.y - upperY, scenelvled->uiCamera.screenSize.x, upperY, (Color) { 0, 0, 0, 128 });

    // Set the font size
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    // The rectangle for the current button to draw
    Rectangle currentButtonRect = {
        .x = buttonOffset,
        .y = scenelvled->uiCamera.screenSize.y - buttonOffset - buttonHeight,
        .width = buttonWidth,
        .height = buttonHeight,
    };

    if (scenelvled->uiMode == EDITOR_UI_MODE_DELETE) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Delete button
    if (GuiButton(currentButtonRect, "Delete")) {
        scenelvled->uiMode = EDITOR_UI_MODE_DELETE;
    }
    if (!shouldLockUI(scenelvled)) GuiUnlock();
    GuiSetAlpha(1);

    // Change the y position for the Edit button
    currentButtonRect.y -= buttonOffset + buttonHeight;

    if (scenelvled->uiMode == EDITOR_UI_MODE_EDIT) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Edit button
    if (GuiButton(currentButtonRect, "Edit")) {
        scenelvled->uiMode = EDITOR_UI_MODE_EDIT;
    }
    if (!shouldLockUI(scenelvled)) GuiUnlock();
    GuiSetAlpha(1);

    // Change the y position for the Build button
    currentButtonRect.y -= buttonOffset + buttonHeight;
    
    if (scenelvled->uiMode == EDITOR_UI_MODE_BUILD) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Build button
    if (GuiButton(currentButtonRect, "Build")) {
        scenelvled->uiMode = EDITOR_UI_MODE_BUILD;
    }
    if (!shouldLockUI(scenelvled)) GuiUnlock();
    GuiSetAlpha(1);

    switch (scenelvled->uiMode) {
        case EDITOR_UI_MODE_BUILD: {
            buttonOffset = convertToScreen(BUTTON_GRID_OFFSET, scenelvled->uiCamera);
            buttonWidth = convertToScreen(BUTTON_GRID_BUTTON_SIZE, scenelvled->uiCamera);
            buttonHeight = buttonWidth;

            Coord buttonGridCenter = {
                .x = 0,
                .y = (BUTTON_GRID_HEIGHT / 2 + BUTTON_GRID_OFFSET) - scenelvled->uiCamera.screenSizeAsCoord.y / 2,
            };

            int row = 0;
            int column = 0;
            int len = NOB_ARRAY_LEN(objectDefenitions);
            for (int i = 0; i < len; ++i) {
                if (!objectDefenitions[i].exists) continue;

                Coord buttonPos = {
                    .x = buttonGridCenter.x + (-BUTTON_GRID_WIDTH/2 + column*BUTTON_GRID_BUTTON_SIZE + column*BUTTON_GRID_OFFSET) + BUTTON_GRID_BUTTON_SIZE/2,
                    .y = buttonGridCenter.y - (-BUTTON_GRID_HEIGHT/2 + row*BUTTON_GRID_BUTTON_SIZE + row*BUTTON_GRID_OFFSET) - BUTTON_GRID_BUTTON_SIZE/2,
                };
                ScreenCoord scButtonPos = getScreenCoord(buttonPos, scenelvled->uiCamera);

                if (i == scenelvled->blockBuildId) {
                    GuiLock();
                } else {
                    GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
                }

                bool clicked = GuiButton((Rectangle) {
                    .x = scButtonPos.x - buttonWidth / 2,
                    .y = scButtonPos.y - buttonHeight / 2,
                    .width = buttonWidth,
                    .height = buttonHeight,
                }, NULL);

                if (!shouldLockUI(scenelvled)) GuiUnlock();
                GuiSetAlpha(1);

                Object buttonObject = {
                    .position = buttonPos,
                    .scale = 30.0 / BUTTON_GRID_BUTTON_SIZE,
                    .angle = 0,
                    .selected = false,
                    .id = i,
                };
                objectDraw(buttonObject, scenelvled->uiCamera);

                if (clicked) {
                    scenelvled->blockBuildId = i;
                }

                ++column;
                if (column >= BUTTON_GRID_COLUMNS) {
                    column = 0;
                    ++row;
                }
            }
        } break;
        case EDITOR_UI_MODE_EDIT: {
            // Draw text telling the user that they're in edit mode
            // This is temporary and will be removed once I add buttons to edit mode
            DrawText("You are in EDIT MODE!", buttonOffset * 3 + buttonWidth, scenelvled->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        } break;
        case EDITOR_UI_MODE_DELETE: {
            // Draw text telling the user that they're in delete mode
            // This is temporary and will be removed once I add buttons to delete mode
            DrawText("You are in DELETE MODE!", buttonOffset * 3 + buttonWidth, scenelvled->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        } break;
    }

    // Draw the pause button and/or menu

    double pauseButtonSize = 20.0;
    double pauseIconSize = 1.0;
    Coord pauseButtonTopLeftCoord = {
        scenelvled->uiCamera.screenSizeAsCoord.x / 2 - pauseButtonSize * 1.5,
        scenelvled->uiCamera.screenSizeAsCoord.y / 2 - pauseButtonSize * 0.5,
    };
    ScreenCoord pauseButtonSCoord = getScreenCoord(pauseButtonTopLeftCoord, scenelvled->uiCamera);
    long pauseButtonSSize = convertToScreen(pauseButtonSize, scenelvled->uiCamera);
    long pauseIconSSize = convertToScreen(pauseIconSize, scenelvled->uiCamera);


    Rectangle pauseButtonRect = {
        pauseButtonSCoord.x, pauseButtonSCoord.y,
        pauseButtonSSize, pauseButtonSSize,
    };

    if (keyboardPressed(KEY_ESCAPE)) {
        scenelvled->isPaused = !scenelvled->isPaused;
    }

    GuiSetIconScale(pauseIconSSize);
    if (GuiButton(pauseButtonRect, GuiIconText(ICON_PLAYER_PAUSE, ""))) {
        scenelvled->isPaused = true;
        scenelvled->clickedButton = true;
    }

    if (scenelvled->isPaused) {
        DrawRectangle(0, 0, scenelvled->uiCamera.screenSize.x, scenelvled->uiCamera.screenSize.y, (Color) { 0, 0, 0, 128 });

        if (!popupIsShown()) GuiUnlock();

        double firstButtonY = (PAUSE_MENU_BUTTON_COUNT / 2) * PAUSE_MENU_BUTTON_HEIGHT + (PAUSE_MENU_BUTTON_COUNT / 2 - 0.5) * PAUSE_MENU_BUTTON_OFFSET;
        Coord firstButtonTopLeftCoord = {
            -PAUSE_MENU_BUTTON_WIDTH / 2,
            firstButtonY + PAUSE_MENU_BUTTON_HEIGHT / 2,
        };
        ScreenCoord firstButtonTopLeftSCoord = getScreenCoord(firstButtonTopLeftCoord, scenelvled->uiCamera);

        Rectangle pauseMenuButtonRect = {
            firstButtonTopLeftSCoord.x,
            firstButtonTopLeftSCoord.y,
            convertToScreen(PAUSE_MENU_BUTTON_WIDTH, scenelvled->uiCamera),
            convertToScreen(PAUSE_MENU_BUTTON_HEIGHT, scenelvled->uiCamera),
        };

        long pauseMenuButtonYIncrement = convertToScreen(PAUSE_MENU_BUTTON_HEIGHT + PAUSE_MENU_BUTTON_OFFSET, scenelvled->uiCamera);

        PauseMenuButton pauseMenuActiveButton = PAUSE_MENU_BUTTON_COUNT;
        for (int i = 0; i < PAUSE_MENU_BUTTON_COUNT; ++i) {
            if (GuiButton(pauseMenuButtonRect, pauseMenuButtonTexts[i])) {
                pauseMenuActiveButton = i;
            }
            pauseMenuButtonRect.y += pauseMenuButtonYIncrement;
        }

        switch (pauseMenuActiveButton) {
            case PAUSE_MENU_BUTTON_RESUME: {
                scenelvled->isPaused = false;
            } break;
            case PAUSE_MENU_BUTTON_SAVE: {
                if (!levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                } else {
                    popupShow("Successfully saved the level!");
                }
            } break;
            case PAUSE_MENU_BUTTON_SAVE_AND_EXIT: {
                if (!levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                } else {
                    popupShow("The level select menu hasn't been implemented yet.\nYour level has been saved, though :)");
                }
            } break;
            case PAUSE_MENU_BUTTON_SAVE_AND_PLAY: {
                if (!levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                } else {
                    sceneswitcherTransitionTo(sceneState, SCENE_LEVEL);
                }
            } break;
            case PAUSE_MENU_BUTTON_COUNT: {
                // No button has been pressed
            } break;
            default: {
                popupShow(POPUP_MESSAGE_NOT_IMPLEMENTED);
            } break;
        }
    }

    // Update the popups
    GuiUnlock();
    popupUpdateUI(scenelvled->uiCamera);
}

void scenelvledDraw(SceneLevelEditor* scenelvled) {
    cameraRecalculateScreenSize(&scenelvled->camera);
    
    // Draw the background
    ClearBackground(scenelvled->levelSettings.backgroundColor);
    // Temporary fix for a weird transparency issue
    DrawRectangle(0, 0, scenelvled->camera.screenSize.x, scenelvled->camera.screenSize.y, scenelvled->levelSettings.backgroundColor);

    // Draw the ground
    drawGround(scenelvled->levelSettings.groundColor, scenelvled->camera);

    // Draw the grid
    if (scenelvled->camera.zoomLevel > -1.5) gridDraw(scenelvled->camera);

    // Draw the white begin and ground lines
    ScreenCoord whitelinesPos = getScreenCoord((Coord){0, GROUND_Y}, scenelvled->camera);

    DrawLine(
        whitelinesPos.x, 0,
        whitelinesPos.x, scenelvled->camera.screenSize.y,
        WHITE
    );
    DrawLine(
        0, whitelinesPos.y,
        scenelvled->camera.screenSize.x, whitelinesPos.y,
        WHITE
    );

    // Draw the objects
    for (size_t i = 0; i < arrlenu(scenelvled->objects); ++i) {
        Object object = scenelvled->objects[i];
        objectDraw(object, scenelvled->camera);
        objectDrawHitbox(object, false, scenelvled->camera);
    }
}
