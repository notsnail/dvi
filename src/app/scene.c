#include "scene.h"
#include "app.h"

#include <stdlib.h>
#include <stddef.h>

static Scene* persistentScene = NULL;
static Scene* scenes[HAL_MAX_SCENES]; // 8 max scenes
static int currentScene = 0;

// exports
void RegisterScene(int index, Scene* scene)
{
    scenes[index] = scene;
    if (index == currentScene) InitScene();
}

void BindScene(int index) // passing -1 to this function quits the application
{
    UnloadScene();
    currentScene = index;

    if (index < 0) 
    {
        AppCleanup();
        exit(0);
    }
    InitScene();
}

// backend
void InitScene()
{
    scenes[currentScene]->init();
    scenes[currentScene]->initialized = 1;
}

void UnloadScene()
{
    scenes[currentScene]->unload();
    scenes[currentScene]->initialized = 0;
}

void DrawScene()
{
    scenes[currentScene]->draw();
}

void UpdateScene()
{
    scenes[currentScene]->update();
}
