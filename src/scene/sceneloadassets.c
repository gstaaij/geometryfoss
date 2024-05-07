#include "sceneloadassets.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "assets/assets.h"
#include "input/keyboard.h"
#include "assets/font.h"
#include "ui/text.h"
#include "scene/sceneswitcher.h"

static char* filesToCopy[] = {
    "Resources/bigFont"ASSET_RESOLUTION_STRING".png",
    "Resources/bigFont"ASSET_RESOLUTION_STRING".fnt",
    "Resources/chatFont"ASSET_RESOLUTION_STRING".png",
    "Resources/chatFont"ASSET_RESOLUTION_STRING".fnt",
    "Resources/GJ_GameSheet"ASSET_RESOLUTION_STRING".png",
    "Resources/GJ_GameSheet"ASSET_RESOLUTION_STRING".plist",
};

static char* fileDestinations[] = {
    "fonts/bigFont"ASSET_RESOLUTION_STRING".png",
    "fonts/bigFont.fnt",
    "fonts/chatFont"ASSET_RESOLUTION_STRING".png",
    "fonts/chatFont.fnt",
    "maps/GJ_GameSheet.png",
    "maps/GJ_GameSheet.plist",
};

SceneLoadAssets* sceneloadassetsCreate() {
    SCENE_CREATE(SceneLoadAssets);

    assert(NOB_ARRAY_LEN(filesToCopy) == NOB_ARRAY_LEN(fileDestinations));

    this->state = SLASTATE_AWAITING_DRAG_DROP;

    this->alreadyHasAssets = true;
    for (size_t i = 0; i < NOB_ARRAY_LEN(fileDestinations); ++i) {
        if (!FileExists(TextFormat("%s/assets/%s", GetApplicationDirectory(), fileDestinations[i]))) {
            this->alreadyHasAssets = false;
            break;
        }
    }

    return this;
}

void sceneloadassetsDestroy(SceneLoadAssets* this) {
    if (this->GDDirectory != NULL)
        free(this->GDDirectory);
    nob_temp_reset();
    free(this);
}

void sceneloadassetsUpdate(SceneLoadAssets* this, SceneState* sceneState, double deltaTime) {
    if (this->alreadyHasAssets) {
        sceneswitcherTransitionEx(sceneState, SCENE_LVLED, 1.0, 0.5);
        return;
    }

    switch (this->state) {
        case SLASTATE_AWAITING_DRAG_DROP: {
            if (IsFileDropped()) {
                FilePathList filePaths = LoadDroppedFiles();
                for (size_t i = 0; i < filePaths.count; ++i) {
                    const char* filePath = filePaths.paths[i];
                    if (strcmp(GetFileName(filePath), "GeometryDash.exe") == 0) {
                        const char* dirPath = GetDirectoryPath(filePath);
                        TraceLog(LOG_INFO, "Geometry Dash directory: %s", dirPath);
                        this->GDDirectory = malloc(sizeof(char) * (strlen(dirPath) + 1));
                        strcpy(this->GDDirectory, dirPath);
                        this->state = SLASTATE_EXTRACTING_ASSETS;
                        return;
                    }
                }
            }
            if (keyboardPressed(KEY_SPACE)) {
                sceneswitcherTransitionTo(sceneState, SCENE_LVLED);
                return;
            }
        } break;
        case SLASTATE_EXTRACTING_ASSETS: {
            if (this->currentAssetIndex >= NOB_ARRAY_LEN(fileDestinations)) {
                if (!sceneState->transition.transitioning)
                    sceneswitcherTransitionTo(sceneState, SCENE_LVLED);
                return;
            }
            const char* srcPath = TextFormat("%s/%s", this->GDDirectory, filesToCopy[this->currentAssetIndex]);
            const char* destPath = TextFormat("%s/assets/%s", GetApplicationDirectory(), fileDestinations[this->currentAssetIndex]);
            const char* assetsDir = TextFormat("%s/assets", GetApplicationDirectory());
            if (!DirectoryExists(assetsDir)) {
                if (!nob_mkdir_if_not_exists(assetsDir)) {
                    this->state = SLASTATE_COPY_ERROR;
                    this->errorMessage = nob_temp_sprintf("Couldn't create directory %s", assetsDir);
                    return;
                }
            }
            if (!DirectoryExists(GetDirectoryPath(destPath))) {
                if (!nob_mkdir_if_not_exists(GetDirectoryPath(destPath))) {
                    this->state = SLASTATE_COPY_ERROR;
                    this->errorMessage = nob_temp_sprintf("Couldn't create directory %s", GetDirectoryPath(TextFormat("assets/%s", fileDestinations[this->currentAssetIndex])));
                    return;
                }
            }
            if (!nob_copy_file(srcPath, destPath)) {
                this->state = SLASTATE_COPY_ERROR;
                this->errorMessage = nob_temp_sprintf("Couldn't copy %s", filesToCopy[this->currentAssetIndex]);
                return;
            }

            this->currentAssetIndex++;
        } break;
        default: {} break;
    }
    (void) deltaTime;
}

void sceneloadassetsUpdateUI(SceneLoadAssets* this, SceneState* sceneState) {
    cameraRecalculateScreenSize(&this->uiCamera);
    (void) sceneState;
}

void sceneloadassetsDraw(SceneLoadAssets* this) {
    ClearBackground(ColorFromHSV(0, 0.0, 0.25));
    if (this->alreadyHasAssets) return;

    switch (this->state) {
        case SLASTATE_AWAITING_DRAG_DROP: {
            textDrawCentered(
                this->uiCamera, fontGetBig(),
                "Drag GeometryDash.exe to this window",
                (Coord) { 0, 0 },
                15, WHITE
            );
            textDrawCentered(
                this->uiCamera, fontGetBig(),
                "or press SPACE to continue without GD assets",
                (Coord) { 0, -20 },
                10, WHITE
            );
        } break;
        case SLASTATE_EXTRACTING_ASSETS: {
            textDrawCentered(
                this->uiCamera, fontGetBig(),
                this->currentAssetIndex >= NOB_ARRAY_LEN(fileDestinations)
                    ? "Done!"
                    : TextFormat("Copying %s...", GetFileName(fileDestinations[this->currentAssetIndex])),
                (Coord) { 0, 0 },
                15, WHITE
            );
        } break;
        case SLASTATE_COPY_ERROR: {
            Coord pos = { 0, 0 };
            textDrawCentered(
                this->uiCamera, fontGetBig(),
                "Copying assets failed",
                pos,
                15, WHITE
            );
            pos.y -= 20;
            if (this->errorMessage != NULL) {
                textDrawCentered(
                    this->uiCamera, fontGetBig(),
                    this->errorMessage,
                    pos,
                    10, WHITE
                );
                pos.y -= 10;
            }
            textDrawCentered(
                this->uiCamera, fontGetBig(),
                "Refer to the console output for more information",
                pos,
                10, LIGHTGRAY
            );
            pos.y -= 10;
        }
    }
}
