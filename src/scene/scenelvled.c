#include "scenelvled.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "stb_ds.h"
#include "raygui.h"
#include "cJSON/cJSON.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "grid.h"
#include "ground.h"
#include "select.h"
#include "camera.h"
#include "serialize.h"
#include "level/level.h"

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
        nob_log(NOB_WARNING, "Couldn't load save! This is likely due to the save not existing yet");

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

    if (scenelvled->isPaused || scenelvled->clickedButton) {
        /// TODO: fix the bug where you place something when clicking the pause button
        scenelvled->clickedButton = false;
        return;
    }
    
    if (mousePressed(MOUSE_BUTTON_LEFT)) {
        startMouseX = GetMouseX();
        startMouseY = GetMouseY();
        holdTime = 0.0;

        isValidClick = convertToGD(scenelvled->camera.screenSize.y - startMouseY, scenelvled->camera) > MOUSE_DEAD_ZONE_UPPER_Y;
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
            case EDITOR_UI_MODE_BUILD:
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
                break;
            case EDITOR_UI_MODE_EDIT:

                selectAddObjectClicked(scenelvled->objects, clickPos, IsKeyDown(KEY_LEFT_SHIFT));

                break;
            case EDITOR_UI_MODE_DELETE:

                for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
                    if (objectMouseOver(scenelvled->objects[i], clickPos)) {
                        arrdel(scenelvled->objects, i);
                        break;
                    }
                }

                break;
            }

        } else if (mouseDown(MOUSE_BUTTON_LEFT)) {
            holdTime += deltaTime;
            int deltaX = GetMouseX() - startMouseX;
            int deltaY = GetMouseY() - startMouseY;
            int mouseDeltaLengthSq = deltaX*deltaX + deltaY*deltaY;
            
            if (mouseDeltaLengthSq > 15625) {
                isDragging = true;
            }
        }
    }

    if (mouseReleased(MOUSE_BUTTON_LEFT)) {
        startMouseX = startMouseY = -1;
        isDragging = false;
    }


    bool shiftDown = keyboardDown(KEY_LEFT_SHIFT);
    bool keyPressedW = keyboardPressed(KEY_W);
    bool keyPressedS = keyboardPressed(KEY_S);
    bool keyPressedA = keyboardPressed(KEY_A);
    bool keyPressedD = keyboardPressed(KEY_D);
    bool keyPressedDel = keyboardPressed(KEY_DELETE);
    bool usefulKeyPressed = keyPressedW || keyPressedS || keyPressedA || keyPressedD || keyPressedDel;
    // Move or delete selected blocks if the correct key is pressed
    if (usefulKeyPressed) {
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
                if (keyPressedDel) {
                    arrdel(scenelvled->objects, i);
                }
            }
        }
    }

    // Save the level
    if (keyboardPressed(KEY_F2)) {
        if (!levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
            nob_log(NOB_ERROR, "Couldn't save the level");
            /// TODO: show a popup to the user telling them the level couldn't be saved
        }
    }

    // Load the level
    if (keyboardPressed(KEY_F3)) {
        if (!levelLoadFromFile("level.json", &scenelvled->levelSettings, &scenelvled->objects))
            nob_log(NOB_ERROR, "Couldn't load save!");
    }

    switch (scenelvled->uiMode) {
    case EDITOR_UI_MODE_BUILD:
        break;
    case EDITOR_UI_MODE_EDIT:
        break;
    case EDITOR_UI_MODE_DELETE:
        break;
    }
}

void scenelvledUpdateUI(SceneLevelEditor* scenelvled, SceneState* sceneState) {
    cameraRecalculateScreenSize(&scenelvled->uiCamera);

    if (scenelvled->isPaused)
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

    // Draw the Delete button
    if (GuiButton(currentButtonRect, "Delete")) {
        scenelvled->uiMode = EDITOR_UI_MODE_DELETE;
    }

    // Change the y position for the Edit button
    currentButtonRect.y -= buttonOffset + buttonHeight;

    // Draw the Edit button
    if (GuiButton(currentButtonRect, "Edit")) {
        scenelvled->uiMode = EDITOR_UI_MODE_EDIT;
    }

    // Change the y position for the Build button
    currentButtonRect.y -= buttonOffset + buttonHeight;
    
    // Draw the Build button
    if (GuiButton(currentButtonRect, "Build")) {
        scenelvled->uiMode = EDITOR_UI_MODE_BUILD;
    }

    switch (scenelvled->uiMode) {
    case EDITOR_UI_MODE_BUILD:
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
                GuiSetAlpha(0.75);
                GuiLock();
            }
            bool clicked = GuiButton((Rectangle) {
                .x = scButtonPos.x - buttonWidth / 2,
                .y = scButtonPos.y - buttonHeight / 2,
                .width = buttonWidth,
                .height = buttonHeight,
            }, NULL);
            if (i == scenelvled->blockBuildId) {
                if (!scenelvled->isPaused) GuiUnlock();
                GuiSetAlpha(1);
            }

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
        break;
    case EDITOR_UI_MODE_EDIT:
        // Draw text telling the user that they're in edit mode
        // This is temporary and will be removed once I add buttons to edit mode
        DrawText("You are in EDIT MODE!", buttonOffset * 3 + buttonWidth, scenelvled->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        break;
    case EDITOR_UI_MODE_DELETE:
        // Draw text telling the user that they're in delete mode
        // This is temporary and will be removed once I add buttons to delete mode
        DrawText("You are in DELETE MODE!", buttonOffset * 3 + buttonWidth, scenelvled->uiCamera.screenSize.y - upperY + buttonOffset, 30, WHITE);
        break;
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

        GuiUnlock();

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
            case PAUSE_MENU_BUTTON_SAVE_AND_PLAY: {
                if (!levelSaveToFile("level.json", scenelvled->levelSettings, scenelvled->objects)) {
                    nob_log(NOB_ERROR, "Couldn't save the level");
                    /// TODO: show a popup to the user telling them the level couldn't be saved
                } else {
                    sceneswitcherTransitionTo(sceneState, SCENE_LEVEL);
                }
            } break;
            case PAUSE_MENU_BUTTON_COUNT: {
                // No button has been pressed
            } break;
            default: {
                assert(0 && "NOT IMPLEMENTED: Pause Menu Button");
            } break;
        }
    }
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
    gridDraw(scenelvled->camera);

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
