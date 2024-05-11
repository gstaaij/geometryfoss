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
#include "assets/assets.h"
#include "assets/font.h"
#include "ui/text.h"
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
    SCENE_CREATE(SceneLevelEditor);

    cameraRecalculateScreenSize(&this->camera);
    this->camera.position.x = this->camera.screenSizeAsCoord.x / 2 - 110;
    this->camera.position.y = this->camera.screenSizeAsCoord.y / 2 - 20;

    this->uiCamera.position = (Coord){0};

    this->objects = NULL;

    this->uiMode = EDITOR_UI_MODE_BUILD;

    this->levelSettings.backgroundColor = GetColor(0x287dffff);
    this->levelSettings.groundColor = GetColor(0x0066ffff);

    // Build Mode

    this->blockBuildId = 1;


    // Load the level
    if (!levelLoadFromFile("level.json", &this->levelSettings, &this->objects))
        TraceLog(LOG_WARNING, "Couldn't load save! This is likely due to the save not existing yet");

    return this;
}
void scenelvledDestroy(SceneLevelEditor* this) {
    free(this);
}


// Some global variables needed for the update loop

static bool isDragging;
static bool isValidClick;
static double holdTime;
static int startMouseX;
static int startMouseY;

void scenelvledUpdate(SceneLevelEditor* this, SceneState* sceneState, double deltaTime) {
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
                if (shouldSave && !levelSaveToFile("level.json", this->levelSettings, this->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                }
            } break;
            case POPUP_QUESTION_LOAD: {
                bool shouldLoad = popupResult == POPUP_BUTTON_ONE;
                if (shouldLoad && !levelLoadFromFile("level.json", &this->levelSettings, &this->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't load save!");
                    popupShow(POPUP_MESSAGE_COULDNT_LOAD);
                }
            } break;
        }
        popupQuestion = POPUP_QUESTION_NONE;
    }

    if (this->isPaused || this->clickedButton) {
        /// TODO: fix the bug where you place something when clicking the pause button
        this->clickedButton = false;
        return;
    }
    
    if (mousePressed(MOUSE_BUTTON_LEFT)) {
        startMouseX = GetMouseX();
        startMouseY = GetMouseY();
        holdTime = 0.0;

        isValidClick = convertToGD(this->uiCamera.screenSize.y - startMouseY, this->uiCamera) > MOUSE_DEAD_ZONE_UPPER_Y;
    }

    if (isValidClick) {
        if (isDragging) {
            holdTime += deltaTime;
            
            Vector2 mouseDelta = mouseGetDelta();
            double deltaXCoord = convertToGD(mouseDelta.x, this->camera);
            double deltaYCoord = convertToGD(mouseDelta.y, this->camera);

            this->camera.position.x -= deltaXCoord;
            this->camera.position.y += deltaYCoord;
        } else if (mouseReleased(MOUSE_BUTTON_LEFT) && !this->clickedButton) {
            
            ScreenCoord clickScreenCoord = {
                .x = GetMouseX(),
                .y = GetMouseY(),
            };
            Coord clickPos = getGDCoord(clickScreenCoord, this->camera);
            switch (this->uiMode) {
                case EDITOR_UI_MODE_BUILD: {
                    ObjectDefinition def = objectDefinitions[this->blockBuildId];
                    clickPos.x = floor(clickPos.x / 30) * 30 + 15 + def.placeOffset.x;
                    clickPos.y = floor(clickPos.y / 30) * 30 + 15 + def.placeOffset.y;
                    Object newObject = {
                        .id = this->blockBuildId,
                        .position = clickPos,
                        .angle = 0,
                        .scale = 1.0,
                        .baseColor = def.baseDefaultColor,
                        .detailColor = def.detailDefaultColor,
                    };
                    arrput(this->objects, newObject);
                    selectAddObjectIndex(this->objects, arrlen(this->objects) - 1, false);
                } break;

                case EDITOR_UI_MODE_EDIT: {
                    selectAddObjectClicked(this->objects, clickPos, IsKeyDown(KEY_LEFT_SHIFT));
                } break;

                case EDITOR_UI_MODE_DELETE: {
                    for (int i = arrlen(this->objects) - 1; i >= 0; --i) {
                        if (objectMouseOver(this->objects[i], clickPos)) {
                            arrdel(this->objects, i);
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
            
            int maxMouseDelta = convertToScreen(30, this->uiCamera);

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
            int selectedObjectsCount = 0;
            int objectId = 0;
            double objectAngle = 0;
            for (int i = arrlen(this->objects) - 1; i >= 0; --i) {
                Object* object = &this->objects[i];
                if (object->selected) {
                    selectedObjectsCount += 1;
                    objectId = object->id;
                    objectAngle = object->angle;
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
            if (selectedObjectsCount == 1) {
                // Make sure objects with a place offset rotate correctly (e.g. slabs, ground spikes/pits, etc.)
                if (objectAngle == 0.0) {
                    selectedObjectRotateAroundPosition.x -= objectDefinitions[objectId].placeOffset.x;
                    selectedObjectRotateAroundPosition.y -= objectDefinitions[objectId].placeOffset.y;
                } else if (objectAngle == 90.0) {
                    selectedObjectRotateAroundPosition.x -= objectDefinitions[objectId].placeOffset.y;
                    selectedObjectRotateAroundPosition.y += objectDefinitions[objectId].placeOffset.x;
                } else if (objectAngle == 180.0) {
                    selectedObjectRotateAroundPosition.x += objectDefinitions[objectId].placeOffset.x;
                    selectedObjectRotateAroundPosition.y += objectDefinitions[objectId].placeOffset.y;
                } else if (objectAngle == 270.0) {
                    selectedObjectRotateAroundPosition.x += objectDefinitions[objectId].placeOffset.y;
                    selectedObjectRotateAroundPosition.y -= objectDefinitions[objectId].placeOffset.x;
                }
            }
            TraceLog(LOG_DEBUG, "Selected object rotation position: {%lf, %lf}", selectedObjectRotateAroundPosition.x, selectedObjectRotateAroundPosition.y);
        }
        for (int i = arrlen(this->objects) - 1; i >= 0; --i) {
            Object* object = &this->objects[i];
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
                    arrdel(this->objects, i);
                }
            }
        }
    }

    // Zooming in
    if (keyboardPressedMod(KEY_EQUAL, false, true)) {
        cameraSetZoomLevel(&this->camera, this->camera.zoomLevel + 0.1);
    }
    // Zooming out
    if (keyboardPressedMod(KEY_MINUS, false, true)) {
        cameraSetZoomLevel(&this->camera, this->camera.zoomLevel - 0.1);
    }
    // Zooming with the mouse
    if (GetMouseWheelMove() != 0 && keyboardDown(KEY_LEFT_CONTROL)) {
        cameraSetZoomLevel(&this->camera, this->camera.zoomLevel + (GetMouseWheelMove() > 0 ? 0.1 : -0.1));
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

    switch (this->uiMode) {
        case EDITOR_UI_MODE_BUILD: {} break;
        case EDITOR_UI_MODE_EDIT: {} break;
        case EDITOR_UI_MODE_DELETE: {} break;
    }
}

bool shouldLockUI(SceneLevelEditor* this) {
    return this->isPaused || popupIsShown();
}

void scenelvledUpdateUI(SceneLevelEditor* this, SceneState* sceneState) {
    cameraRecalculateScreenSize(&this->uiCamera);

    if (shouldLockUI(this))
        GuiLock();

    // Convert some sizes from GD coordinates to screen coordinates

    long buttonOffset = convertToScreen(MODE_BUTTON_OFFSET, this->uiCamera);
    long buttonWidth = convertToScreen(MODE_BUTTON_WIDTH, this->uiCamera);
    long buttonHeight = convertToScreen(MODE_BUTTON_HEIGHT, this->uiCamera);
    long fontSize = convertToScreen(MODE_BUTTON_FONT_SIZE, this->uiCamera);
    long upperY = convertToScreen(MOUSE_DEAD_ZONE_UPPER_Y, this->uiCamera);

    // Draw the bottom button pane
    DrawRectangle(0, this->uiCamera.screenSize.y - upperY, this->uiCamera.screenSize.x, upperY, (Color) { 0, 0, 0, 128 });

    // Set the font size
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    // The rectangle for the current button to draw
    Rectangle currentButtonRect = {
        .x = buttonOffset,
        .y = this->uiCamera.screenSize.y - buttonOffset - buttonHeight,
        .width = buttonWidth,
        .height = buttonHeight,
    };

    if (this->uiMode == EDITOR_UI_MODE_DELETE) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Delete button
    if (GuiButton(currentButtonRect, "Delete")) {
        this->uiMode = EDITOR_UI_MODE_DELETE;
    }
    if (!shouldLockUI(this)) GuiUnlock();
    GuiSetAlpha(1);

    // Change the y position for the Edit button
    currentButtonRect.y -= buttonOffset + buttonHeight;

    if (this->uiMode == EDITOR_UI_MODE_EDIT) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Edit button
    if (GuiButton(currentButtonRect, "Edit")) {
        this->uiMode = EDITOR_UI_MODE_EDIT;
    }
    if (!shouldLockUI(this)) GuiUnlock();
    GuiSetAlpha(1);

    // Change the y position for the Build button
    currentButtonRect.y -= buttonOffset + buttonHeight;
    
    if (this->uiMode == EDITOR_UI_MODE_BUILD) {
        GuiLock();
    } else {
        GuiSetAlpha(DESELECTED_BUTTON_ALPHA);
    }
    // Draw the Build button
    if (GuiButton(currentButtonRect, "Build")) {
        this->uiMode = EDITOR_UI_MODE_BUILD;
    }
    if (!shouldLockUI(this)) GuiUnlock();
    GuiSetAlpha(1);

    switch (this->uiMode) {
        case EDITOR_UI_MODE_BUILD: {
            buttonOffset = convertToScreen(BUTTON_GRID_OFFSET, this->uiCamera);
            buttonWidth = convertToScreen(BUTTON_GRID_BUTTON_SIZE, this->uiCamera);
            buttonHeight = buttonWidth;

            Coord buttonGridCenter = {
                .x = 0,
                .y = (BUTTON_GRID_HEIGHT / 2 + BUTTON_GRID_OFFSET) - this->uiCamera.screenSizeAsCoord.y / 2,
            };

            int row = 0;
            int column = 0;
            int len = NOB_ARRAY_LEN(objectDefinitions);
            for (int i = 0; i < len; ++i) {
                if (!objectDefinitions[i].exists) continue;

                Coord buttonPos = {
                    .x = buttonGridCenter.x + (-BUTTON_GRID_WIDTH/2 + column*BUTTON_GRID_BUTTON_SIZE + column*BUTTON_GRID_OFFSET) + BUTTON_GRID_BUTTON_SIZE/2,
                    .y = buttonGridCenter.y - (-BUTTON_GRID_HEIGHT/2 + row*BUTTON_GRID_BUTTON_SIZE + row*BUTTON_GRID_OFFSET) - BUTTON_GRID_BUTTON_SIZE/2,
                };
                ScreenCoord scButtonPos = getScreenCoord(buttonPos, this->uiCamera);

                if (i == this->blockBuildId) {
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

                if (!shouldLockUI(this)) GuiUnlock();
                GuiSetAlpha(1);

                Object buttonObject = {
                    .position = buttonPos,
                    .scale = 35.0 / BUTTON_GRID_BUTTON_SIZE / fmax(objectDefinitions[i].shape.scaleX, objectDefinitions[i].shape.scaleY),
                    .angle = 0,
                    .selected = false,
                    .id = i,
                    .baseColor = objectDefinitions[i].baseDefaultColor,
                    .detailColor = GetColor(0xc8c8ffff),
                };
                objectDraw(buttonObject, false, this->uiCamera);

                if (clicked) {
                    this->blockBuildId = i;
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
            textDraw(fontGetBig(), "You are in EDIT MODE!", buttonOffset * 3 + buttonWidth, this->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        } break;
        case EDITOR_UI_MODE_DELETE: {
            // Draw text telling the user that they're in delete mode
            // This is temporary and will be removed once I add buttons to delete mode
            textDraw(fontGetBig(), "You are in DELETE MODE!", buttonOffset * 3 + buttonWidth, this->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        } break;
    }

    // Draw the pause button and/or menu

    double pauseButtonSize = 20.0;
    double pauseIconSize = 1.0;
    Coord pauseButtonTopLeftCoord = {
        this->uiCamera.screenSizeAsCoord.x / 2 - pauseButtonSize * 1.5,
        this->uiCamera.screenSizeAsCoord.y / 2 - pauseButtonSize * 0.5,
    };
    ScreenCoord pauseButtonSCoord = getScreenCoord(pauseButtonTopLeftCoord, this->uiCamera);
    long pauseButtonSSize = convertToScreen(pauseButtonSize, this->uiCamera);
    long pauseIconSSize = convertToScreen(pauseIconSize, this->uiCamera);


    Rectangle pauseButtonRect = {
        pauseButtonSCoord.x, pauseButtonSCoord.y,
        pauseButtonSSize, pauseButtonSSize,
    };

    if (keyboardPressed(KEY_ESCAPE)) {
        this->isPaused = !this->isPaused;
    }

    GuiSetIconScale(pauseIconSSize);
    if (GuiButton(pauseButtonRect, GuiIconText(ICON_PLAYER_PAUSE, ""))) {
        this->isPaused = true;
        this->clickedButton = true;
    }

    if (this->isPaused) {
        DrawRectangle(0, 0, this->uiCamera.screenSize.x, this->uiCamera.screenSize.y, (Color) { 0, 0, 0, 128 });

        if (!popupIsShown()) GuiUnlock();

        double firstButtonY = (PAUSE_MENU_BUTTON_COUNT / 2) * PAUSE_MENU_BUTTON_HEIGHT + (PAUSE_MENU_BUTTON_COUNT / 2 - 0.5) * PAUSE_MENU_BUTTON_OFFSET;
        Coord firstButtonTopLeftCoord = {
            -PAUSE_MENU_BUTTON_WIDTH / 2,
            firstButtonY + PAUSE_MENU_BUTTON_HEIGHT / 2,
        };
        ScreenCoord firstButtonTopLeftSCoord = getScreenCoord(firstButtonTopLeftCoord, this->uiCamera);

        Rectangle pauseMenuButtonRect = {
            firstButtonTopLeftSCoord.x,
            firstButtonTopLeftSCoord.y,
            convertToScreen(PAUSE_MENU_BUTTON_WIDTH, this->uiCamera),
            convertToScreen(PAUSE_MENU_BUTTON_HEIGHT, this->uiCamera),
        };

        long pauseMenuButtonYIncrement = convertToScreen(PAUSE_MENU_BUTTON_HEIGHT + PAUSE_MENU_BUTTON_OFFSET, this->uiCamera);

        PauseMenuButton pauseMenuActiveButton = PAUSE_MENU_BUTTON_COUNT;
        for (int i = 0; i < PAUSE_MENU_BUTTON_COUNT; ++i) {
            if (GuiButton(pauseMenuButtonRect, pauseMenuButtonTexts[i])) {
                pauseMenuActiveButton = i;
            }
            pauseMenuButtonRect.y += pauseMenuButtonYIncrement;
        }

        switch (pauseMenuActiveButton) {
            case PAUSE_MENU_BUTTON_RESUME: {
                this->isPaused = false;
            } break;
            case PAUSE_MENU_BUTTON_SAVE: {
                if (!levelSaveToFile("level.json", this->levelSettings, this->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                } else {
                    popupShow("Successfully saved the level!");
                }
            } break;
            case PAUSE_MENU_BUTTON_SAVE_AND_EXIT: {
                if (!levelSaveToFile("level.json", this->levelSettings, this->objects)) {
                    TraceLog(LOG_ERROR, "Couldn't save the level");
                    popupShow(POPUP_MESSAGE_COULDNT_SAVE);
                } else {
                    popupShow("The level select menu hasn't been implemented yet.\nYour level has been saved, though :)");
                }
            } break;
            case PAUSE_MENU_BUTTON_SAVE_AND_PLAY: {
                if (!levelSaveToFile("level.json", this->levelSettings, this->objects)) {
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
    popupUpdateUI(this->uiCamera);
}

void scenelvledDraw(SceneLevelEditor* this) {
    cameraRecalculateScreenSize(&this->camera);
    
    // Draw the background
    ClearBackground(this->levelSettings.backgroundColor);
    // Temporary fix for a weird transparency issue
    DrawRectangle(0, 0, this->camera.screenSize.x, this->camera.screenSize.y, this->levelSettings.backgroundColor);

    // Draw the ground
    drawGround(this->levelSettings.groundColor, this->camera);

    // Draw the grid
    if (this->camera.zoomLevel > -1.5) gridDraw(this->camera);

    // Draw the white begin and ground lines
    ScreenCoord whitelinesPos = getScreenCoord((Coord){0, GROUND_Y}, this->camera);

    DrawLine(
        whitelinesPos.x, 0,
        whitelinesPos.x, this->camera.screenSize.y,
        WHITE
    );
    DrawLine(
        0, whitelinesPos.y,
        this->camera.screenSize.x, whitelinesPos.y,
        WHITE
    );

    // Draw the objects
    for (size_t i = 0; i < arrlenu(this->objects); ++i) {
        Object object = this->objects[i];
        objectDraw(object, false, this->camera);
        objectDrawHitbox(object, false, this->camera);
    }

#ifdef DEBUG
    // Draw a test texture
    const char* fileName = "blackCogwheel_02_001.png";
    const char* fileNameColor = "blackCogwheel_02_color_001.png";
    const char* fileNameGlow = "blackCogwheel_02_glow_001.png";
    TextureMap map = assetsTextureMap(fileName);
    TextureMap mapColor = assetsTextureMap(fileNameColor);
    TextureMap mapGlow = assetsTextureMap(fileNameGlow);
    Coord position = { 15, 15 };
    double rotation = GetTime() * 180.0;
    BeginBlendMode(BLEND_ADDITIVE);
        assetsDrawFromTextureMap(mapGlow, position, 1.0, rotation, ColorFromHSV(0, 0.0, 0.5), this->camera);
    EndBlendMode();
    assetsDrawFromTextureMap(mapColor, position, 1.0, rotation, BLACK, this->camera);
    assetsDrawFromTextureMap(map, position, 1.0, rotation, WHITE, this->camera);
#endif
}
