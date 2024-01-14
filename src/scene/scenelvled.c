#include "scenelvled.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "stb_ds.h"
#include "nob.h"
#include "raygui.h"
#include "grid.h"
#include "ground.h"
#include "select.h"
#include "camera.h"

#define MODE_BUTTON_OFFSET 5.0
#define MOUSE_DEAD_ZONE_UPPER_Y 90
#define MODE_BUTTON_WIDTH 75.0
#define MODE_BUTTON_HEIGHT ((MOUSE_DEAD_ZONE_UPPER_Y - MODE_BUTTON_OFFSET*4) / 3)

#define BUTTON_GRID_COLUMNS 6.0
#define BUTTON_GRID_ROWS 2.0

#define BUTTON_GRID_OFFSET 5.0
#define BUTTON_GRID_HEIGHT (MOUSE_DEAD_ZONE_UPPER_Y - 2.0 * BUTTON_GRID_OFFSET)
#define BUTTON_GRID_BUTTON_SIZE (BUTTON_GRID_HEIGHT / BUTTON_GRID_ROWS - (BUTTON_GRID_ROWS * BUTTON_GRID_OFFSET) / 4)
#define BUTTON_GRID_WIDTH (BUTTON_GRID_COLUMNS * BUTTON_GRID_BUTTON_SIZE + (BUTTON_GRID_COLUMNS - 1) * BUTTON_GRID_OFFSET)

SceneLevelEditor* scenelvledCreate() {
    SceneLevelEditor* scenelvled = (SceneLevelEditor*) malloc(sizeof(SceneLevelEditor));
    assert(scenelvled != NULL && "You don't have enough RAM");
    memset(scenelvled, 0, sizeof(SceneLevelEditor));

    cameraRecalculateScreenSize(&scenelvled->camera);
    scenelvled->camera.position.x = scenelvled->camera.screenSizeAsCoord.x / 2 - 110;
    scenelvled->camera.position.y = scenelvled->camera.screenSizeAsCoord.y / 2 - 20;

    scenelvled->objects = NULL;
    scenelvled->selectedObjects = NULL;

    scenelvled->uiMode = EDITOR_UI_MODE_BUILD;

    scenelvled->backgroundColor = GetColor(0x287dffff);
    scenelvled->groundColor = GetColor(0x0066ffff);

    return scenelvled;
}
void scenelvledDestroy(SceneLevelEditor* scenelvled) {
    free(scenelvled);
}

bool isDragging;
bool isValidClick;
double holdTime;
int startMouseX;
int startMouseY;

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startMouseX = GetMouseX();
        startMouseY = GetMouseY();
        holdTime = 0.0;

        isValidClick = convertToGD(scenelvled->camera.screenSize.y - startMouseY, scenelvled->camera) > MOUSE_DEAD_ZONE_UPPER_Y;
    }

    if (isValidClick) {
        if (isDragging) {
            holdTime += deltaTime;
            
            Vector2 mouseDelta = GetMouseDelta();
            double deltaXCoord = convertToGD(mouseDelta.x, scenelvled->camera);
            double deltaYCoord = convertToGD(mouseDelta.y, scenelvled->camera);

            scenelvled->camera.position.x -= deltaXCoord;
            scenelvled->camera.position.y += deltaYCoord;
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            
            /// TODO: do more stuff
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
                    .id = 1,
                    .position = clickPos,
                    .angle = 0,
                    .scale = 1.0,
                };
                arrput(scenelvled->objects, newObject);
                selectAddObjectIndex(scenelvled->objects, arrlen(scenelvled->objects) - 1, false);
                break;
            case EDITOR_UI_MODE_EDIT:

                selectAddObjectClicked(scenelvled->objects, clickPos, IsKeyDown(KEY_LEFT_SHIFT));

                // for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
                //     if (objectMouseOver(scenelvled->objects[i], clickPos)) {
                //         if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                //             arrfree(scenelvled->selectedObjects);
                //             scenelvled->selectedObjects = NULL;
                //         }
                //         arrput(scenelvled->selectedObjects, i);
                //         break;
                //     }
                // }

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

        } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            holdTime += deltaTime;
            int deltaX = GetMouseX() - startMouseX;
            int deltaY = GetMouseY() - startMouseY;
            int mouseDeltaLengthSq = deltaX*deltaX + deltaY*deltaY;
            
            if (mouseDeltaLengthSq > 15625) {
                isDragging = true;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        startMouseX = startMouseY = -1;
        isDragging = false;
    }



    bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT);
    bool keyPressedW = IsKeyPressed(KEY_W);
    bool keyPressedS = IsKeyPressed(KEY_S);
    bool keyPressedA = IsKeyPressed(KEY_A);
    bool keyPressedD = IsKeyPressed(KEY_D);
    bool keyPressedDel = IsKeyPressed(KEY_DELETE);
    bool usefulKeyPressed = keyPressedW || keyPressedS || keyPressedA || keyPressedD || keyPressedDel;
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

    switch (scenelvled->uiMode) {
    case EDITOR_UI_MODE_BUILD:
        break;
    case EDITOR_UI_MODE_EDIT:
        break;
    case EDITOR_UI_MODE_DELETE:
        break;
    }
}

void scenelvledUpdateUI(SceneLevelEditor* scenelvled) {
    long buttonOffset = convertToScreen(MODE_BUTTON_OFFSET, scenelvled->camera);
    long buttonWidth = convertToScreen(MODE_BUTTON_WIDTH, scenelvled->camera);
    long buttonHeight = convertToScreen(MODE_BUTTON_HEIGHT, scenelvled->camera);
    long fontSize = convertToScreen(18, scenelvled->camera);
    long upperY = convertToScreen(MOUSE_DEAD_ZONE_UPPER_Y, scenelvled->camera);

    DrawRectangle(0, scenelvled->camera.screenSize.y - upperY, scenelvled->camera.screenSize.x, upperY, (Color) { 0, 0, 0, 128 });

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);

    Rectangle currentButtonRect = {
        .x = buttonOffset,
        .y = scenelvled->camera.screenSize.y - buttonOffset - buttonHeight,
        .width = buttonWidth,
        .height = buttonHeight,
    };

    if (GuiButton(currentButtonRect, "Delete")) {
        scenelvled->uiMode = EDITOR_UI_MODE_DELETE;
    }

    currentButtonRect.y -= buttonOffset + buttonHeight;

    if (GuiButton(currentButtonRect, "Edit")) {
        scenelvled->uiMode = EDITOR_UI_MODE_EDIT;
    }

    currentButtonRect.y -= buttonOffset + buttonHeight;
    
    if (GuiButton(currentButtonRect, "Build")) {
        scenelvled->uiMode = EDITOR_UI_MODE_BUILD;
    }

    switch (scenelvled->uiMode) {
    case EDITOR_UI_MODE_BUILD:
        buttonOffset = convertToScreen(BUTTON_GRID_OFFSET, scenelvled->camera);
        buttonWidth = convertToScreen(BUTTON_GRID_BUTTON_SIZE, scenelvled->camera);
        buttonHeight = buttonWidth;

        double buttonGridCenterX = scenelvled->camera.screenSizeAsCoord.x / 2;
        double buttonGridCenterY = scenelvled->camera.screenSizeAsCoord.y - (BUTTON_GRID_HEIGHT / 2 + BUTTON_GRID_OFFSET);

        DrawRectangle(
            convertToScreen(buttonGridCenterX - BUTTON_GRID_WIDTH / 2, scenelvled->camera),
            convertToScreen(buttonGridCenterY - BUTTON_GRID_HEIGHT / 2, scenelvled->camera),
            convertToScreen(BUTTON_GRID_WIDTH, scenelvled->camera),
            convertToScreen(BUTTON_GRID_HEIGHT, scenelvled->camera),
            RED
        );

        int row = 0;
        int column = 0;
        for (size_t i = 0; i < BUTTON_GRID_COLUMNS * BUTTON_GRID_ROWS; ++i) {
            // if (!objectDefenitions[i].exists) continue;
            double buttonX = buttonGridCenterX + (-BUTTON_GRID_WIDTH/2 + column*BUTTON_GRID_BUTTON_SIZE + column*BUTTON_GRID_OFFSET) + BUTTON_GRID_BUTTON_SIZE/2;
            double buttonY = buttonGridCenterY + (-BUTTON_GRID_HEIGHT/2 + row*BUTTON_GRID_BUTTON_SIZE + row*BUTTON_GRID_OFFSET) + BUTTON_GRID_BUTTON_SIZE/2;
            long x = convertToScreen(buttonX, scenelvled->camera);
            long y = convertToScreen(buttonY, scenelvled->camera);
            GuiButton((Rectangle) {
                .x = x - buttonWidth / 2,
                .y = y - buttonHeight / 2,
                .width = buttonWidth,
                .height = buttonHeight,
            }, "");
            ++column;
            if (column >= BUTTON_GRID_COLUMNS) {
                column = 0;
                ++row;
            }
        }
        break;
    case EDITOR_UI_MODE_EDIT:
        DrawText("You are in EDIT MODE!", scenelvled->camera.screenSize.x / 2, scenelvled->camera.screenSize.y / 2, 30, WHITE);
        break;
    case EDITOR_UI_MODE_DELETE:
        DrawText("You are in DELETE MODE!", scenelvled->camera.screenSize.x / 2, scenelvled->camera.screenSize.y / 2, 30, WHITE);
        break;
    }
}

void scenelvledDraw(SceneLevelEditor* scenelvled) {
    cameraRecalculateScreenSize(&scenelvled->camera);
    

    ClearBackground(scenelvled->backgroundColor);

    drawGround(scenelvled->groundColor, scenelvled->camera);

    gridDraw(scenelvled->camera);

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
