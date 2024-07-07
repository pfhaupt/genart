#include <stdlib.h>
#include <stdio.h>

#include "plug.h"

#include <raylib.h>

#define SHADER_PATH "./shaders/mandelbrot.fs"

typedef struct {
    u64 lastChanged;
    Shader shader;
    RenderTexture2D target;
} Plug;

static Plug *p = NULL;

__declspec(dllexport) void plug_init(void) {
    p = malloc(sizeof(*p));
    assert(p != NULL && "Buy more RAM lol");
    memset(p, 0, sizeof(*p));

    p->shader = LoadShader(NULL, SHADER_PATH);
    p->target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    p->lastChanged = get_last_modified(SHADER_PATH);
}

__declspec(dllexport) void *plug_pre_reload(void) {
    return p;
}

__declspec(dllexport) void plug_post_reload(void *pp) {
    p = pp;
}

__declspec(dllexport) void plug_update(void) {
    reload_shader_if_necessary(&p->shader, &p->lastChanged, SHADER_PATH);
    if (IsWindowResized()) {
        UnloadRenderTexture(p->target);
        p->target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }
    BeginDrawing();
    ClearBackground(BLACK);
    SetShaderValue(
        p->shader,
        GetShaderLocation(p->shader, "resolution"),
        (float[]){GetScreenWidth(), GetScreenHeight()},
        SHADER_UNIFORM_VEC2
    );
    BeginShaderMode(p->shader);
    DrawTexture(p->target.texture, 0, 0, WHITE);
    EndShaderMode();
    EndDrawing();
}
