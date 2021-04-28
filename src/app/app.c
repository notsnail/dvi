#include "app.h"

#include <stdlib.h>
#include <raylib.h>
#include "scene.h"
#include "../util/error.h"

RenderTexture _render_texture;

void AppInit(const char* app_name, int magnification)
{
    // top secret coconut nut verification system
    if (!FileExists("img/coconut.jpg"))
    {
        TheGameIsLackingCoconutError();
    }

    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(HAL_INTERNAL_WIDTH * magnification, HAL_INTERNAL_HEIGHT * magnification, app_name);
    SetTraceLogLevel(LOG_WARNING);
    SetExitKey(0);

    _render_texture = LoadRenderTexture(HAL_INTERNAL_WIDTH, HAL_INTERNAL_HEIGHT);
    SetTextureFilter(_render_texture.texture, FILTER_POINT);
}

void AppCleanup()
{
    UnloadRenderTexture(_render_texture);
    CloseWindow();
}

static void AppUpdate()
{
    // Update
    UpdateScene();

    // Draw     
    BeginTextureMode(_render_texture);
        DrawScene();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(_render_texture.texture,
                        (Rectangle) { 0, 0, _render_texture.texture.width, -_render_texture.texture.height },
                        (Rectangle) { 0, 0, GetScreenWidth() , GetScreenHeight() },
                        (Vector2) { 0, 0 },
                        0.0f,
                        WHITE);
    EndDrawing();
}

int AppMain()
{
    while (!WindowShouldClose())
    {
        AppUpdate();
    }

    return 0;
}
