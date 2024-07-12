#ifndef PLUG_H_
#define PLUG_H_

#include <assert.h>
#include <stdlib.h>

#include <raylib.h>

#define LIST_OF_PLUGS \
    PLUG(plug_init, void, void) \
    PLUG(plug_pre_reload, void*, void) \
    PLUG(plug_post_reload, void, void*) \
    PLUG(plug_update, void, void)

#define PLUG(name, ret, ...) typedef ret (name##_t)(__VA_ARGS__);
LIST_OF_PLUGS
#undef PLUG

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#else
#error "Add support for non-Windows"
#endif // _WIN32
typedef unsigned long long u64;

u64 get_last_modified(const char*);
void reload_shader_if_necessary(Shader*, u64*, const char*);

void reload_shader_if_necessary(Shader* s, u64* l, const char* filepath) {
    u64 time = get_last_modified(filepath);
    if (time > *l) {
        *l = time;
        if (!ReloadShader(s, NULL, filepath)) {
            TraceLog(LOG_ERROR, "Could not load shader %s.", filepath);
        }
    }
}

u64 get_last_modified(const char* file_path) {
#ifdef _WIN32
    HANDLE file = CreateFile(file_path, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        TraceLog(LOG_FATAL, "Could not get the last modified time of file %s: %lu\n", file_path, GetLastError());
    }
    FILETIME written;
    if (!GetFileTime(file, NULL, NULL, &written)) {
        TraceLog(LOG_FATAL, "Could not get file time: %lu\n", GetLastError());
    }
    bool _r = CloseHandle(file);
    assert(_r);
    return *(u64*)&written;
#else
#error "Add support for non-Windows"
#endif // _WIN32
}

#endif // PLUG_H_
