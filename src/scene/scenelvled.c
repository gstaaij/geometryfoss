#include "scenelvled.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "stb_ds.h"
#include "raygui.h"
#include "ground.h"
#include "camera.h"

#define MODE_BUTTON_OFFSET 5
#define MODE_BUTTON_WIDTH 65
#define MODE_BUTTON_HEIGHT 20
#define MOUSE_DEAD_ZONE_UPPER_Y (3 * MODE_BUTTON_HEIGHT + 4 * MODE_BUTTON_OFFSET)

SceneLevelEditor* scenelvledCreate() {
    SceneLevelEditor* scenelvled = (SceneLevelEditor*) malloc(sizeof(SceneLevelEditor));
    assert(scenelvled != NULL && "You don't have enough RAM");
    memset(scenelvled, 0, sizeof(SceneLevelEditor));

    cameraRecalculateScreenSize(&scenelvled->camera);
    scenelvled->camera.position.x = scenelvled->camera.screenSizeAsCoord.x / 2 - 60;
    scenelvled->camera.position.y = scenelvled->camera.screenSizeAsCoord.y / 2;

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
                    .id = 8,
                    .position = clickPos,
                    .angle = 0,
                    .scale = 1.0,
                };
                arrput(scenelvled->objects, newObject);
                break;
            case EDITOR_UI_MODE_EDIT:

                for (int i = arrlen(scenelvled->objects) - 1; i >= 0; --i) {
                    if (objectMouseOver(scenelvled->objects[i], clickPos)) {
                        if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                            arrfree(scenelvled->selectedObjects);
                            scenelvled->selectedObjects = NULL;
                        }
                        arrput(scenelvled->selectedObjects, i);
                        break;
                    }
                }

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



    switch (scenelvled->uiMode) {
    case EDITOR_UI_MODE_BUILD:
        break;
    case EDITOR_UI_MODE_EDIT:

        for (int i = arrlen(scenelvled->selectedObjects) - 1; i >= 0; --i) {
            size_t index = scenelvled->selectedObjects[i];
            bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT);
            if (IsKeyPressed(KEY_W)) {
                scenelvled->objects[index].position.y += 2 + !shiftDown * 28;
            }
            if (IsKeyPressed(KEY_S)) {
                scenelvled->objects[index].position.y -= 2 + !shiftDown * 28;
            }
            if (IsKeyPressed(KEY_A)) {
                scenelvled->objects[index].position.x -= 2 + !shiftDown * 28;
            }
            if (IsKeyPressed(KEY_D)) {
                scenelvled->objects[index].position.x += 2 + !shiftDown * 28;
            }
            if (IsKeyPressed(KEY_DELETE)) {
                /// TODO: this is broken, it deletes things and then doesn't consider that things move around, but it works for just one object
                arrdel(scenelvled->objects, index);
                arrdel(scenelvled->selectedObjects, i);
            }
        }

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
        DrawText("You are in BUILD MODE!", scenelvled->camera.screenSize.x / 2, scenelvled->camera.screenSize.y / 2, 30, WHITE);
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

    ScreenCoord whitelinesPos = getScreenCoord((Coord){0, GROUND_Y}, scenelvled->camera);

    // Draw the objects
    for (size_t i = 0; i < arrlenu(scenelvled->objects); ++i) {
        Object object = scenelvled->objects[i];
        objectDraw(object, scenelvled->camera);
        for (size_t j = 0; j < arrlenu(scenelvled->selectedObjects); ++j) {
            if (i == scenelvled->selectedObjects[j]) {
                BeginBlendMode(BLEND_MULTIPLIED);
                    // Get the Object Defenition tied to this Object
                    ObjectDefinition def = objectDefenitions[object.id];

                    // Calculate the size of the block based on the Object Defenition and the Object's scale
                    double scale = def.shape.scale * object.scale;
                    double blockSize = scale * 30;

                    // Convert some values to Screen Coordinates
                    ScreenCoord scBlock = getScreenCoord(object.position, scenelvled->camera);
                    long scBlockSize = convertToScreen(blockSize, scenelvled->camera);

                    DrawRectangle(
                        scBlock.x - scBlockSize / 2,
                        scBlock.y - scBlockSize / 2,
                        scBlockSize,
                        scBlockSize,
                        GREEN
                    );
                EndBlendMode();
                break;
            }
        }
        objectDrawHitbox(object, true, scenelvled->camera);
    }

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
}
