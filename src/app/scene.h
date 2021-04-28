#pragma once
#include <raylib.h>

#define HAL_MAX_SCENES 8

typedef void (*_scene_callback)();

typedef struct
{
    _scene_callback init;
    _scene_callback update;
    _scene_callback draw;
    _scene_callback unload;

    int initialized;
} Scene;

// exports

extern void RegisterScene(int index, Scene* scene);
extern void BindScene(int index);

extern void InitScene();
extern void UnloadScene();
extern void DrawScene();
extern void UpdateScene();
