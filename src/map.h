#ifndef HAL_MAP_H

typedef struct 
{
    const char* name, author;
} hal_map;

extern hal_map hal_load_map(const char* filename);
extern void hal_draw_map();
extern void hal_unload_map(hal_map* map);

#define HAL_MAP_H
#endif