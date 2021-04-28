#ifndef HAL_SPRITE_H

#include <raylib.h>

typedef struct
{
    char* name;
    Rectangle* sources;
    int frame_count;
    int speed;
} Animation;

typedef struct
{
    // texture
    Texture2D spritesheet;

    // data 
    char* name;
    Animation* animations;
    int total_animations;

    // flags
    int current_animation;
    int frame_counter;
    int tick;
} Sprite;

extern Sprite LoadSprite(const char* filename); // takes in .sfs file
extern void UnloadSprite(Sprite* sprite);

extern void SpriteSwitchToString(Sprite* sprite, const char* anim_id);
extern void SpriteBindIndex(Sprite* sprite, int index);
extern void UpdateSprite(Sprite* sprite);
extern void DrawSprite(Sprite* sprite, Vector2 where);

#define HAL_SPRITE_H
#endif