#include "scenelvled.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>

SceneLevelEditor* scenelvledCreate() {
    SceneLevelEditor* scenelvled = (SceneLevelEditor*) malloc(sizeof(SceneLevelEditor));
    assert(scenelvled != NULL && "You don't have enough RAM");
    memset(scenelvled, 0, sizeof(SceneLevelEditor));
    return scenelvled;
}
void scenelvledDestroy(SceneLevelEditor* scenelvled) {
    free(scenelvled);
}

void scenelvledUpdate(SceneLevelEditor* scenelvled, double deltaTime) {
    (void) scenelvled;
    (void) deltaTime;
    assert("NOT IMPLEMENTED: scenelvledUpdate");
}

void scenelvledDraw(SceneLevelEditor* scenelvled) {
    (void) scenelvled;
    assert("NOT IMPLEMENTED: scenelvledDraw");
}
