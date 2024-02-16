#pragma once
#include "raylib.h"


typedef enum {
    SCENE_NONE = 0,
    SCENE_CRASH,
    SCENE_LEVEL,
    SCENE_LVLED,
} SceneEnum; // I'm very good at naming things

typedef struct {
    // Whether the scene is transitioning
    bool transitioning;

    // The scene that should be loaded
    SceneEnum targetScene;
    // Whether the next scene should be loaded
    // It's up to the scene manager to actually load the scene
    bool shouldLoadTargetScene;
    // Whether the next scene has been loaded
    // It's up to the scene manager to call sceneswitcherLoadedNewScene to set this flag when the scene has been loaded
    bool hasLoadedTargetScene;

    // The time the transition takes in seconds
    double duration;
    // The time the transition has been going on for in seconds
    double time;
    // A value between 0.0 and 1.0 indicating how far along the transition is
    double progress;
} SceneTransition;

typedef struct {
    SceneEnum currentScene;
    SceneTransition transition;
} SceneState;

SceneState* sceneswitcherCreateState();
void sceneswitcherDestroyState(SceneState* state);

// Transition to a scene with a transition duration of 1.0
void sceneswitcherTransitionTo(SceneState* state, const SceneEnum nextScene);
// Transition to a scene with a specific transition duration, starting at a specific time
void sceneswitcherTransitionEx(SceneState* state, const SceneEnum nextScene, const double duration, const double startTime);

// Returns whether the next scene should be loaded
bool sceneswitcherShouldLoadNewScene(SceneState* state);
// To be run by the scene manager when the next scene has been loaded, to make sure the transition continues
void sceneswitcherLoadedNewScene(SceneState* state);

// Update the transition state
void sceneswitcherUpdate(SceneState* state, const double deltaTime);
// Draw the scene transition
void sceneswitcherUpdateUI(SceneState* state);
