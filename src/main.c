#include <stdlib.h>
#include <stdio.h>

#include <raylib.h>

#include "plug.h"
#define NOB_IMPLEMENTATION
#include "nob.h"

#define PLUG(name, ...) name##_t *name = NULL;
LIST_OF_PLUGS
#undef PLUG

static void *libplug = NULL;

bool reload_libplug(char* lib) {
    TraceLog(LOG_INFO, "Reloading plugin");
    if (libplug != NULL) FreeLibrary(libplug);

    libplug = LoadLibrary(lib);
    if (libplug == NULL) {
        TraceLog(LOG_ERROR, "HOTRELOAD: could not load %s: %lu", lib, GetLastError());
        return false;
    }

    #define PLUG(name, ...) \
        name = (void*)GetProcAddress(libplug, #name); \
        if (name == NULL) { \
            TraceLog(LOG_ERROR, "HOTRELOAD: could not find %s symbol in %s: %lu", \
                     #name, lib, GetLastError()); \
            return false; \
        }
    LIST_OF_PLUGS
    #undef PLUG

    return true;
}

int main(int argc, char** argv) {
    (void)nob_shift_args(&argc, &argv); // program name

    if (argc == 0) {
        fprintf(stderr, "error: Expected dll as argument");
        return 1;
    }

    char* lib = nob_shift_args(&argc, &argv);
    if (!reload_libplug(lib)) return 1;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TOPMOST);
    size_t factor = 80;
    InitWindow(factor*16, factor*9, "Shader Shenanigans");
    SetTargetFPS(60);

    plug_init();
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_H)) {
            void *state = plug_pre_reload();
            if (!reload_libplug(lib)) return 1;
            plug_post_reload(state);
        }
        plug_update();
    }

    CloseWindow();

    return 0;
}
