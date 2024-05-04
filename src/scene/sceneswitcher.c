#include "sceneswitcher.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include "util.h"

SceneState* sceneswitcherCreateState() {
    SCENE_CREATE(SceneState);
    return this;
}
void sceneswitcherDestroyState(SceneState* state) {
    free(state);
}


void sceneswitcherTransitionTo(SceneState* state, const SceneEnum nextScene) {
    sceneswitcherTransitionEx(state, nextScene, 1.0, 0.0);
}

void sceneswitcherTransitionEx(SceneState* state, const SceneEnum nextScene, const double duration, const double startTime) {
    state->transition.transitioning = true;
    state->transition.shouldLoadTargetScene = false;
    state->transition.hasLoadedTargetScene = false;
    state->transition.targetScene = nextScene;
    state->transition.duration = duration;
    state->transition.time = startTime;
    state->transition.progress = 0.0;
}

bool sceneswitcherShouldLoadNewScene(SceneState* state) {
    return state->transition.shouldLoadTargetScene && !state->transition.hasLoadedTargetScene;
}

void sceneswitcherLoadedNewScene(SceneState* state) {
    state->transition.hasLoadedTargetScene = true;
    state->currentScene = state->transition.targetScene;
    state->transition.targetScene = SCENE_NONE;
}

void sceneswitcherUpdate(SceneState* state, const double deltaTime) {
    if (!state->transition.transitioning)
        return;
    SceneTransition* transition = &state->transition;
    if (sceneswitcherShouldLoadNewScene(state))
        return;
    transition->time += deltaTime;
    transition->progress = transition->time / transition->duration;
    if (transition->progress >= 0.5 && !transition->shouldLoadTargetScene) {
        transition->progress = 0.5;
        transition->shouldLoadTargetScene = true;
        transition->hasLoadedTargetScene = false;
    } else if (transition->progress >= 1.0) {
        transition->transitioning = false;
    }
}

void sceneswitcherUpdateUI(SceneState* state) {
    if (!state->transition.transitioning)
        return;
    
    long alpha = (long) (state->transition.progress * 255 * 2);
    if (alpha > 255) {
        alpha -= 255;
        alpha = 255 - alpha;
    }
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color) { 0, 0, 0, alpha });
}
