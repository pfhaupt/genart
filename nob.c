#define NOB_IMPLEMENTATION
#include "nob.h"

static const char *raylib_modules[] = {
    "rcore",
    "raudio",
    "rglfw",
    "rmodels",
    "rshapes",
    "rtext",
    "rtextures",
    "utils",
};

static const char* important_files[] = {
    "plug.h"
};

#define RAYLIB_VERSION "5.0"

bool needs_rebuild(const char* output, Nob_File_Paths input, const char** other) {
    if (nob_needs_rebuild(output, input.items, input.count)) return true;
    if (nob_needs_rebuild(output, other, NOB_ARRAY_LEN(other))) return true;
    return false;
}

bool build_raylib()
{
    bool result = true;
    Nob_Cmd cmd = {0};
    Nob_File_Paths object_files = {0};
    Nob_Procs procs = {0};

    const char *build_path = "./build/raylib";
    if (!nob_mkdir_if_not_exists(build_path)) {
        nob_return_defer(false);
    }

    for (size_t i = 0; i < NOB_ARRAY_LEN(raylib_modules); ++i) {
        const char *input_path = nob_temp_sprintf("./raylib/raylib-"RAYLIB_VERSION"/src/%s.c", raylib_modules[i]);
        const char *output_path = nob_temp_sprintf("%s/%s.o", build_path, raylib_modules[i]);
        output_path = nob_temp_sprintf("%s/%s.o", build_path, raylib_modules[i]);

        nob_da_append(&object_files, output_path);

        if (nob_needs_rebuild(output_path, &input_path, 1)) {
            cmd.count = 0;
            nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
            nob_cmd_append(&cmd, "-ggdb", "-DPLATFORM_DESKTOP", "-fPIC");
            nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src/external/glfw/include");
            nob_cmd_append(&cmd, "-c", input_path);
            nob_cmd_append(&cmd, "-o", output_path);

            Nob_Proc proc = nob_cmd_run_async(cmd);
            nob_da_append(&procs, proc);
        }
    }
    cmd.count = 0;

    if (!nob_procs_wait(procs)) nob_return_defer(false);

    const char *libraylib_path = "./build/raylib.dll";
    if (nob_needs_rebuild(libraylib_path, object_files.items, object_files.count)) {
        nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
        nob_cmd_append(&cmd, "-shared");
        nob_cmd_append(&cmd, "-o", libraylib_path);
        for (size_t i = 0; i < NOB_ARRAY_LEN(raylib_modules); ++i) {
            const char *input_path = nob_temp_sprintf("%s/%s.o", build_path, raylib_modules[i]);
            nob_cmd_append(&cmd, input_path);
        }
        nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");
        if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);
    }

defer:
    nob_cmd_free(cmd);
    nob_da_free(object_files);
    return result;
}

bool build_main() {
    Nob_File_Paths input = {0};
    nob_cmd_append(&input, "./src/main.c");

    const char* output = "./build/main.exe";
    if (needs_rebuild(output, input, important_files)) {
        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
        nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
        nob_cmd_append(&cmd, "-I.");
        nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src/");
        nob_cmd_append(&cmd, "-o", output);
        nob_da_append_many(&cmd, input.items, input.count);
        nob_cmd_append(&cmd, "-Wl,-rpath=./build/", "-Wl,-rpath=./", "-Wl,-rpath=./build/raylib/");
        nob_cmd_append(&cmd, "-L./build", "-l:raylib.dll");
        nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");

        return nob_cmd_run_sync(cmd);
    } else {
        nob_log(NOB_INFO, "%s is up to date.", output);
    }
    return true;
}

bool build_shader(char* filename) {
    Nob_File_Paths input = {0};
    nob_cmd_append(&input, nob_temp_sprintf("./src/%s.c", filename));

    const char* output = nob_temp_sprintf("./build/%s.dll", filename);
    if (needs_rebuild(output, input, important_files)) {
        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
        nob_cmd_append(&cmd, "-mwindows", "-Wall", "-Wextra", "-ggdb");
        nob_cmd_append(&cmd, "-I.");
        nob_cmd_append(&cmd, "-I./raylib/raylib-"RAYLIB_VERSION"/src/");
        nob_cmd_append(&cmd, "-fPIC", "-shared");
        nob_cmd_append(&cmd, "-static-libgcc");
        nob_cmd_append(&cmd, "-o", output);
        nob_da_append_many(&cmd, input.items, input.count);
        nob_cmd_append(&cmd, "-L./build", "-l:raylib.dll");
        nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");

        return nob_cmd_run_sync(cmd);
    } else {
        nob_log(NOB_INFO, "%s is up to date.", output);
    }
    return true;
}

void run_main() {
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "./build/main.exe");
    nob_cmd_run_sync(cmd);
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists("./build")) return 1;
    if (!build_raylib()) return 1;
    if (!build_shader("mandelbrot")) return 1;
    if (!build_main()) return 1;

    nob_log(NOB_INFO, "Done.");
    return 0;
}