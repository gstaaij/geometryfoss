
#define NOB_IMPLEMENTATION
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

void logAvailableTargets(Nob_Log_Level level) {
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
    Nob_Cmd cmd = {0};
    Nob_File_Paths objectFiles = {0};

    if (!nob_mkdir_if_not_exists("./build/raylib")) {
        nob_return_defer(false);
    }

    Nob_Procs procs = {0};

    const char* buildPath = nob_temp_sprintf("./build/raylib/%s", NOB_ARRAY_GET(targetNames, target));

    if (!nob_mkdir_if_not_exists(buildPath)) {
        nob_return_defer(false);
    }

    for (size_t i = 0; i < NOB_ARRAY_LEN(raylibModules); ++i) {
        const char* inputPath = nob_temp_sprintf("./raylib/raylib-"RAYLIB_VERSION"/src/%s.c", raylibModules[i]);
        const char* outputPath = nob_temp_sprintf("%s/%s.o", buildPath, raylibModules[i]);

        nob_da_append(&objectFiles, outputPath);

        if (nob_needs_rebuild1(outputPath, inputPath)) {
            cmd.count = 0;
            switch (target) {
            case TARGET_LINUX:
                nob_cmd_append(&cmd, "gcc");
                break;
            case TARGET_WIN32_MINGW:
                nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
                break;
            default: NOB_ASSERT(0 && "unreachable");
            }
            nob_cmd_append(&cmd, "-ggdb", "-DPLATFORM_DESKTOP", "-fPIC"); // Remove -ggdb for a miniscule amount of extra performance
            nob_cmd_append(&cmd, "-O2");
            nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src/external/glfw/include");
            nob_cmd_append(&cmd, "-c", inputPath);
            nob_cmd_append(&cmd, "-o", outputPath);

            Nob_Proc proc = nob_cmd_run_async(cmd);
            nob_da_append(&procs, proc);
        }
    }
    cmd.count = 0;

    if (!nob_procs_wait(procs)) nob_return_defer(false);

    switch (target) {
    case TARGET_WIN32_MINGW:
    case TARGET_LINUX:
        const char* libraylibPath = nob_temp_sprintf("%s/libraylib.a", buildPath);

        if (nob_needs_rebuild(libraylibPath, objectFiles.items, objectFiles.count)) {
            nob_cmd_append(&cmd, "ar", "-crs", libraylibPath);
            for (size_t i = 0; i < NOB_ARRAY_LEN(raylibModules); ++i) {
                const char* inputPath = nob_temp_sprintf("%s/%s.o", buildPath, raylibModules[i]);
                nob_cmd_append(&cmd, inputPath);
            }
            if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);
        }
        break;
    default: NOB_ASSERT(0 && "unreachable");
    }

defer:
    nob_cmd_free(cmd);
    nob_da_free(objectFiles);
    return result;
}

static const char* cFiles[] = {
    "geometryfoss.c",
    "camera.c",
    "coord.c",
    "cJSON/cJSON.c",
    "grid.c",
    "ground.c",
    "hitbox.c",
    "input/keyboard.c",
    "input/mouse.c",
    "level/level.c",
    "level/levelsettings.c",
    "object.c",
    "player.c",
    "scene/scenemanager.c",
    "scene/scenelevel.c",
    "scene/scenelvled.c",
    "select.c",
    "util.c",
};

bool buildMain(Target target) {
    bool result = true;

    Nob_Cmd cmd = {0};
    switch (target) {
    case TARGET_LINUX:
        cmd.count = 0;
            nob_cmd_append(&cmd, "gcc");
            nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");

            // Disable warnings from raygui
            nob_cmd_append(&cmd, "-isystem", "./src/ray");

            // Disable some warnings from stb_ds
            nob_cmd_append(&cmd, "-isystem", "./src/stb");
            nob_cmd_append(&cmd, "-Wno-missing-field-initializers");
            
            nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src");
            nob_cmd_append(&cmd, "-I./src");
            nob_cmd_append(&cmd, "-I./src/nob");
            nob_cmd_append(&cmd, "-o", "./build/"EXECUTABLE_NAME);

            for (size_t i = 0; i < NOB_ARRAY_LEN(cFiles); ++i) {
                nob_cmd_append(&cmd, nob_temp_sprintf("src/%s", cFiles[i]));
            }
            
            nob_cmd_append(&cmd,
                nob_temp_sprintf("-L./build/raylib/%s", NOB_ARRAY_GET(targetNames, target)),
                "-l:libraylib.a");
            nob_cmd_append(&cmd, "-lm");
        if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);
        break;
    case TARGET_WIN32_MINGW:
        cmd.count = 0;
            nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
            nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb", "-static");

            // Disable some warnings from stb_ds
            nob_cmd_append(&cmd, "-isystem", "./src/stb");
            
            nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src");
            nob_cmd_append(&cmd, "-I./src");
            nob_cmd_append(&cmd, "-I./src/nob");
            nob_cmd_append(&cmd, "-o", "./build/"EXECUTABLE_NAME);

            for (size_t i = 0; i < NOB_ARRAY_LEN(cFiles); ++i) {
                nob_cmd_append(&cmd, nob_temp_sprintf("src/%s", cFiles[i]));
            }
            
            nob_cmd_append(&cmd,
                nob_temp_sprintf("-L./build/raylib/%s", NOB_ARRAY_GET(targetNames, target)),
                "-l:libraylib.a");
            nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");
            nob_cmd_append(&cmd, "-static");
        if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);
        break;
    default: NOB_ASSERT(0 && "unreachable");
    }

defer:
    nob_cmd_free(cmd);
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
        nob_log(NOB_ERROR, "Unknown target %s", value);
        logAvailableTargets(NOB_ERROR);
        return false;
    }

    return true;
}

void logAvailableSubcommands(const char* program, Nob_Log_Level level) {
    nob_log(level, "Usage: %s <subcommand>", program);
    nob_log(level, "Subcommands:");
    nob_log(level, "    build");
    nob_log(level, "    help");
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char* program = nob_shift_args(&argc, &argv);

    if (argc < 1) {
        logAvailableSubcommands(program, NOB_ERROR);
        return 1;
    }

    const char* subcommand = nob_shift_args(&argc, &argv);

    if (strcmp(subcommand, "help") == 0) {
        logAvailableSubcommands(program, NOB_INFO);
        return 0;
    } else if (strcmp(subcommand, "build") == 0) {
        if (argc < 1) {
            nob_log(NOB_INFO, "To compile for a different target, use: %s build <target>", program);
        }

        if (!nob_mkdir_if_not_exists("./build")) return 1;

        // Set default target
    #ifdef _WIN32
        Target target = TARGET_WIN32_MINGW;
    #else
        Target target = TARGET_LINUX;
    #endif // _WIN32

        if (argc >= 1) {
            const char* value = nob_shift_args(&argc, &argv);
            if (!parseTarget(value, &target)) return 1;
        }

        if (!buildRaylib(target)) return 1;
        if (!buildMain(target)) return 1;

    } else {
        logAvailableSubcommands(program, NOB_ERROR);
        return 1;
    }

    return 0;
}