#include "app.h"

#include <stdlib.h>
#include <raylib.h>

#include "scene.h"
#include "../hal/error.h"
#include "../vendor/raylib-physfs.h"

RenderTexture _render_texture;

#define PHYSFS_ERROR -1212

void AppInit(const char* app_name, int magnification)
{
    // open virtual fs and open base package.
    if (!InitPhysFS())
    {
        FatalErrorWithCode("physfs didnt initialize!\n", PHYSFS_ERROR);
    }
    MountPhysFS("halcyon.pak", NULL); // this is temporary as i dont have a .pak file in there yet

    // top secret coconut nut verification system
    if (!FileExistsInPhysFS("img/coconut.jpg"))
    {
        TheGameIsLackingCoconutError();
    }
    else
    {
        printf("found the coconut.\n");
    }

    SetTraceLogLevel(LOG_WARNING);
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

    ClosePhysFS();
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
