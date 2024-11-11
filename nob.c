
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "src/nob/nob.h"

#define EXECUTABLE_NAME "geometryfoss"
#define RAYLIB_VERSION "5.0"

// Stolen from https://github.com/tsoding/musializer
typedef enum {
    TARGET_LINUX,
    TARGET_WIN32_MINGW,
    COUNT_TARGETS
} Target;

static_assert(2 == COUNT_TARGETS, "Amount of targets has changed");
const char *targetNames[] = {
    [TARGET_LINUX]       = "linux",
    [TARGET_WIN32_MINGW] = "win32-mingw",
};

void logAvailableTargets(Log_Level level) {
    nob_log(level, "Available targets:");
    for (size_t i = 0; i < COUNT_TARGETS; ++i) {
        nob_log(level, "    %s", targetNames[i]);
    }
}
////

static const char* raylibModules[] = {
    "rcore",
    "rglfw",
    "rshapes",
    "rtext",
    "rtextures",
    "utils",
};

bool buildRaylib(Target target) {
    // Mostly stolen from https://github.com/tsoding/musializer
    bool result = true;
    Cmd cmd = {0};
    File_Paths objectFiles = {0};

    if (!mkdir_if_not_exists("./build/raylib")) {
        return_defer(false);
    }

    Procs procs = {0};

    const char* buildPath = temp_sprintf("./build/raylib/%s", ARRAY_GET(targetNames, target));

    if (!mkdir_if_not_exists(buildPath)) {
        return_defer(false);
    }

    for (size_t i = 0; i < ARRAY_LEN(raylibModules); ++i) {
        const char* inputPath = temp_sprintf("./raylib/raylib-"RAYLIB_VERSION"/src/%s.c", raylibModules[i]);
        const char* outputPath = temp_sprintf("%s/%s.o", buildPath, raylibModules[i]);

        da_append(&objectFiles, outputPath);

        if (needs_rebuild1(outputPath, inputPath)) {
            switch (target) {
                case TARGET_LINUX: {
                    cmd_append(&cmd, "gcc");
                } break;
                case TARGET_WIN32_MINGW: {
                    cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
                } break;

                default: NOB_ASSERT(0 && "unreachable");
            }
            cmd_append(&cmd, "-ggdb", "-DPLATFORM_DESKTOP", "-D_GNU_SOURCE", "-fPIC"); // Remove -ggdb for a miniscule amount of extra performance
            cmd_append(&cmd, "-O2");
            cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src/external/glfw/include");
            cmd_append(&cmd, "-c", inputPath);
            cmd_append(&cmd, "-o", outputPath);

            Proc proc = cmd_run_async_and_reset(&cmd);
            da_append(&procs, proc);
        }
    }

    if (!procs_wait_and_reset(&procs)) return_defer(false);

    switch (target) {
        case TARGET_WIN32_MINGW:
        case TARGET_LINUX: {
            const char* libraylibPath = temp_sprintf("%s/libraylib.a", buildPath);

            if (needs_rebuild(libraylibPath, objectFiles.items, objectFiles.count)) {
                cmd_append(&cmd, "ar", "-crs", libraylibPath);
                for (size_t i = 0; i < ARRAY_LEN(raylibModules); ++i) {
                    const char* inputPath = temp_sprintf("%s/%s.o", buildPath, raylibModules[i]);
                    cmd_append(&cmd, inputPath);
                }
                if (!cmd_run_sync_and_reset(&cmd)) return_defer(false);
            }
        } break;
        
        default: NOB_ASSERT(0 && "unreachable");
    }

defer:
    cmd_free(cmd);
    da_free(objectFiles);
    return result;
}

static const char* cFiles[] = {
    "geometryfoss.c",
    "assets/assets.c",
    "assets/font.c",
    "camera.c",
    "coord.c",
    "lib/cJSON/cJSON.c",
    "lib/easing/easing.c",
    "lib/yxml/yxml.c",
    "grid.c",
    "ground.c",
    "hitbox.c",
    "input/keyboard.c",
    "input/mouse.c",
    "level/level.c",
    "level/levelsettings.c",
    "object.c",
    "player/player.c",
    "player/physics/playerphysics.c",
    "scene/scenemanager.c",
    "scene/sceneswitcher.c",
    "scene/sceneloadassets.c",
    "scene/scenelevel.c",
    "scene/scenelvled.c",
    "select.c",
    "ui/popup.c",
    "ui/text.c",
};

// Kind of stolen from the get_git_hash function in https://github.com/rexim/tore/blob/main/nob.c
bool addGitVersion(Cmd* cmd) {
    Cmd tempCmd = {0};
    bool result = true;

    Fd fdout = fd_open_for_write("./build/gitHash.txt");
    if (fdout == INVALID_FD) return_defer(false);
    
    cmd_append(&tempCmd, "git", "describe", "--abbrev=7", "--dirty", "--always", "--tags");
    if (!cmd_run_sync_redirect_and_reset(&tempCmd, (Cmd_Redirect) {
        .fdout = &fdout,
    })) return_defer(false);

    String_Builder sb = {0};
    if (!read_entire_file("./build/gitHash.txt", &sb)) return_defer(false);
    while (sb.count > 0 && isspace(sb.items[sb.count - 1])) --sb.count;
    sb_append_null(&sb);

    cmd_append(cmd, temp_sprintf("-DGIT_VERSION=\"%s\"", sb.items));
    int isDirty = strstr(sb.items, "-dirty") != NULL;
    cmd_append(cmd, temp_sprintf("-DGIT_IS_DIRTY=%d", isDirty));
    da_free(sb);

defer:
    if (!result) {
        cmd_append(cmd, "-DGIT_VERSION=\"unknown\"", "-DGIT_IS_DIRTY=0");
    }
    cmd_free(tempCmd);
    return result;
}

bool buildMain(Target target, bool debugMode) {
    bool result = true;

    Cmd cmd = {0};

    switch (target) {
        case TARGET_LINUX: {
            cmd_append(&cmd, "gcc");
            cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");

            if (debugMode)
                cmd_append(&cmd, "-DDEBUG");

            // Add the git version
            addGitVersion(&cmd);

            // Disable warnings from raygui
            cmd_append(&cmd, "-isystem", "./src/lib/ray");

            // Disable some warnings from stb_ds
            cmd_append(&cmd, "-isystem", "./src/lib/stb");
            
            cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src");
            cmd_append(&cmd, "-I./src");
            cmd_append(&cmd, "-I./src/nob");
            cmd_append(&cmd, "-o", "./build/"EXECUTABLE_NAME);

            for (size_t i = 0; i < ARRAY_LEN(cFiles); ++i) {
                cmd_append(&cmd, temp_sprintf("src/%s", cFiles[i]));
            }
            
            cmd_append(&cmd,
                temp_sprintf("-L./build/raylib/%s", ARRAY_GET(targetNames, target)),
                "-l:libraylib.a");
            cmd_append(&cmd, "-lm");
            if (!cmd_run_sync_and_reset(&cmd)) return_defer(false);
        } break;

        case TARGET_WIN32_MINGW: {
            cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
            cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb", "-static");

            if (debugMode)
                cmd_append(&cmd, "-DDEBUG");

            // Add the git version
            addGitVersion(&cmd);
            
            // Disable warnings from raygui
            cmd_append(&cmd, "-isystem", "./src/lib/ray");

            // Disable some warnings from stb_ds
            cmd_append(&cmd, "-isystem", "./src/lib/stb");
            
            cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src");
            cmd_append(&cmd, "-I./src");
            cmd_append(&cmd, "-I./src/nob");
            cmd_append(&cmd, "-o", "./build/"EXECUTABLE_NAME);

            for (size_t i = 0; i < ARRAY_LEN(cFiles); ++i) {
                cmd_append(&cmd, temp_sprintf("src/%s", cFiles[i]));
            }
            
            cmd_append(&cmd,
                temp_sprintf("-L./build/raylib/%s", ARRAY_GET(targetNames, target)),
                "-l:libraylib.a");
            cmd_append(&cmd, "-lwinmm", "-lgdi32");
            cmd_append(&cmd, "-static");
            if (!cmd_run_sync_and_reset(&cmd)) return_defer(false);
        } break;

        default: NOB_ASSERT(0 && "unreachable");
    }

defer:
    cmd_free(cmd);
    return result;
}

bool parseTarget(const char* value, Target* target) {
    bool found = false;
    for (size_t i = 0; !found && i < COUNT_TARGETS; ++i) {
        if (strcmp(targetNames[i], value) == 0) {
            *target = i;
            found = true;
        }
    }

    if (!found) {
        nob_log(ERROR, "Unknown target %s", value);
        logAvailableTargets(ERROR);
        return false;
    }

    return true;
}

void logAvailableSubcommands(const char* program, Log_Level level) {
    nob_log(level, "Usage: %s <subcommand>", program);
    nob_log(level, "Subcommands:");
    nob_log(level, "    build");
    nob_log(level, "    help");
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char* program = shift(argv, argc);

    if (argc < 1) {
        logAvailableSubcommands(program, ERROR);
        return 1;
    }

    const char* subcommand = shift(argv, argc);

    if (strcmp(subcommand, "help") == 0) {
        logAvailableSubcommands(program, INFO);
        return 0;
    } else if (strcmp(subcommand, "build") == 0) {
        if (argc < 1) {
            nob_log(INFO, "To compile for a different target, use: %s build <target> [options]", program);
        }

        if (!mkdir_if_not_exists("./build")) return 1;

        // Set default target
    #ifdef _WIN32
        Target target = TARGET_WIN32_MINGW;
    #else
        Target target = TARGET_LINUX;
    #endif // _WIN32

        if (argc >= 1) {
            const char* value = shift(argv, argc);
            if (!parseTarget(value, &target)) return 1;
        }

        bool debugMode = false;
        while (argc >= 1) {
            const char* arg = shift(argv, argc);
            if (strcmp(arg, "--debug") == 0) {
                debugMode = true;
            } else {
                nob_log(ERROR, "Usage: %s build %s [options]", program, targetNames[target]);
                nob_log(ERROR, "Available options:");
                nob_log(ERROR, "    --debug        Extra debug visuals and logs");
                return 1;
            }
        }

        if (!buildRaylib(target)) return 1;
        if (!buildMain(target, debugMode)) return 1;

    } else {
        logAvailableSubcommands(program, ERROR);
        return 1;
    }

    return 0;
}