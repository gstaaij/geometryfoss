#include "scenelvled.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "stb_ds.h"
#include "ground.h"
#include "camera.h"

SceneLevelEditor* scenelvledCreate() {
    SceneLevelEditor* scenelvled = (SceneLevelEditor*) malloc(sizeof(SceneLevelEditor));
    assert(scenelvled != NULL && "You don't have enough RAM");
    memset(scenelvled, 0, sizeof(SceneLevelEditor));

    cameraRecalculateScreenSize(&scenelvled->camera);
    scenelvled->camera.position.x = scenelvled->camera.screenSizeAsCoord.x / 2 - 60;
    scenelvled->camera.position.y = scenelvled->camera.screenSizeAsCoord.y / 2;

    return scenelvled;
}
void scenelvledDestroy(SceneLevelEditor* scenelvled) {
    free(scenelvled);
}

int isDragging;
double holdTime;
int startMouseX;
int startMouseY;

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        startMouseX = GetMouseX();
        startMouseY = GetMouseY();
        holdTime = 0.0;
    }

    if (isDragging) {
        holdTime += deltaTime;
        
        Vector2 mouseDelta = GetMouseDelta();
        double deltaXCoord = convertToGD(mouseDelta.x, scenelvled->camera);
        double deltaYCoord = convertToGD(mouseDelta.y, scenelvled->camera);

        scenelvled->camera.position.x -= deltaXCoord;
        scenelvled->camera.position.y += deltaYCoord;
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        
        /// TODO: do stuff
        ScreenCoord clickScreenCoord = {
            .x = GetMouseX(),
            .y = GetMouseY(),
        };
        Coord clickPos = getGDCoord(clickScreenCoord, scenelvled->camera);
        clickPos.x = floor(clickPos.x / 30) * 30 + 15;
        clickPos.y = floor(clickPos.y / 30) * 30 + 15;
        Object newObject = {
            .id = 1,
            .position = clickPos,
            .angle = 0,
            .scale = 1.0,
        };
        arrput(scenelvled->objects, newObject);

    } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        holdTime += deltaTime;
        int deltaX = GetMouseX() - startMouseX;
        int deltaY = GetMouseY() - startMouseY;
        int mouseDeltaLengthSq = deltaX*deltaX + deltaY*deltaY;
        
        if (mouseDeltaLengthSq > 15625) {
            isDragging = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        startMouseX = startMouseY = -1;
        isDragging = false;
    } 
}

void scenelvledDraw(SceneLevelEditor* scenelvled) {
    cameraRecalculateScreenSize(&scenelvled->camera);
    

    ClearBackground(BLACK);

    ScreenCoord whitelinesPos = getScreenCoord((Coord){0, GROUND_Y}, scenelvled->camera);

    // Draw the objects
    for (size_t i = 0; i < arrlenu(scenelvled->objects); ++i) {
        objectDraw(scenelvled->objects[i], scenelvled->camera);
        objectDrawHitbox(scenelvled->objects[i], true, scenelvled->camera);
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
