#include "mod_play.h"
#include "app/app.h"
#include <math.h>

#include "gfx/sprite.h"
#include "hal/error.h"

#include "hal/haldefs.h"

#include "vendor/raylib-physfs.h"

Camera2D camera = { 0 };
Sprite player_sprite;

Texture2D background;

struct {
    Vector2 where;
} player;

float offset_x = 0;

float lower = -104;
float upper = 64;

static void init()
{
    background = LoadTextureFromPhysFS("img/sagat.png");

    camera.zoom = 1.0f;
    camera.offset = (Vector2) { HAL_INTERNAL_WIDTH / 2, HAL_INTERNAL_HEIGHT / 2 };
    camera.rotation = 0.0f;

    player.where = (Vector2) { 0, 80 };
    player_sprite = LoadSprite("img/ryu.sprite");

    SetTargetFPS(60);
}

static void update()
{
    int delta_x = 0;

    if (IsKeyPressed(KEY_P)) ToggleFullscreen();

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
    {
        ReportErrorToWindow("test error", HALCYON_ERROR);
    }

    if (IsKeyDown(KEY_LEFT))
    {
        delta_x -= 1;
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        delta_x += 1;
    }

    player.where.x += delta_x;

    if (delta_x != 0) SpriteBindIndex(&player_sprite, 1);
    else SpriteBindIndex(&player_sprite, 0);

    camera.target = (Vector2) { 
        player.where.x,
        player.where.y - 40
    };

    if (camera.target.x < (float) lower )
    {
        camera.target.x = (float) lower;
    }

    if (camera.target.x > (float) upper)
    {
        camera.target.x = (float) upper;
    }

    UpdateSprite(&player_sprite);
}

static void draw()
{
    ClearBackground(BLACK);
    BeginMode2D(camera);
        ClearBackground(BLACK);
        // parallax
        DrawTextureRec(background, (Rectangle) { 0, 0, 512, 128 }, (Vector2) { -256 + offset_x, -64 }, WHITE);

        //normal
        DrawTextureRec(background, (Rectangle) { 0, 128, 512, 64 }, (Vector2) { -256 , 96 - 64}, WHITE);
        DrawTextureRec(background, (Rectangle) { 0, 240, 512, 112 }, (Vector2) { -256, -16 - 64 }, WHITE);
        DrawTextureRec(background, (Rectangle) { 0, 192, 512, 48 }, (Vector2) { -256, 160 - 64 }, WHITE);

        // floor
        DrawTexturePro(background, (Rectangle) { 0, 192, 512, 48 }, 
                        (Rectangle) { -256, 160, 512, 64 }, 
                        (Vector2) { 0, 0 },
                        0.0f, WHITE);

        // top layer
        DrawTextureRec(background, (Rectangle) { 0, 352, 512, 464 }, (Vector2) { 0, 160 }, WHITE);

        // Player
        DrawSprite(&player_sprite, player.where);
    EndMode2D();

    DrawText(TextFormat("%i", GetFPS()), 1, 1, 5, GREEN);
}

static void unload()
{
    UnloadSprite(&player_sprite);
    UnloadTexture(background); 
}

Scene PlayExport = {
    .init = init,
    .update = update,
    .draw = draw,
    .unload = unload,
    .initialized = 0
};