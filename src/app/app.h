#pragma once

#define HAL_INTERNAL_WIDTH 256
#define HAL_INTERNAL_HEIGHT 224

extern void AppInit(const char* app_name, int magnification);
extern void AppCleanup();

extern int AppMain();