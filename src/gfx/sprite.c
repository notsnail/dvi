#include "sprite.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "../hal/haldefs.h"

#include "../vendor/raylib-physfs.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int QuickStoi(const char* str)
{
    int val = 0;
    while (*str) {
        val = val * 10 + (*str++ - '0');
    }
    return val;
}

static Rectangle StringToRectangle(const char* str)
{
    int coords[4] = { 0, 0, 0, 0 };
    int count = 0;

    char* token = strtok(str, " ");
    coords[count] = QuickStoi(token);
    count++;

    for (token = strtok(NULL, " "); token != NULL; token = strtok(NULL, " "))
    {
        coords[count] = QuickStoi(token);
        count++;
    }
    return (Rectangle) { coords[0], coords[1], coords[2], coords[3] };
}

static int CountChildren(xmlNode* node)
{
    int count = 0;
    for (xmlNode* each = node->children; each; each = each->next)
    {
        count++;
    }

    return count;
}

static int CountChildElementsOfType(xmlNode* node, unsigned char* data_type)
{
    int count = 0;
    for (xmlNode* each = node->children; each; each = each->next)
    {
        if (each->type == XML_ELEMENT_NODE)
        {
            if (strcmp(data_type, each->name) == 0)
            {
                count++;
            }            
        }
    }

    return count;
}

static int CountChildElements(xmlNode* node)
{
    int count = 0;
    for (xmlNode* each = node->children; each; each = each->next)
    {
        if (each->type == XML_ELEMENT_NODE) 
        {
            count++;
        }
    }

    return count;
}

Sprite LoadSprite(const char* filename)
{
    #if HAL_DEBUG_FEATURES
    printf("-------> START SPRITE LOAD <-------\n");
    clock_t start = clock();
    #endif

    // initialize parser
    LIBXML_TEST_VERSION
    xmlInitParser();

    // load document
    int doc_str_size;
    unsigned char* doc_str = LoadFileDataFromPhysFS(filename, &doc_str_size);

    xmlDocPtr document = xmlParseDoc(doc_str);
    if (document == NULL) // validate
    {
        printf("failed to open xml file\n");
        return (Sprite) { 0 };
    }
    UnloadFileData(doc_str);

    // start parsing
    xmlNode* root = xmlDocGetRootElement(document);
    if (root == NULL)
    {
        return (Sprite) { 0 };
    }

    // set return value
    Sprite sprite;    

    // set up initial variables
    sprite.name = xmlGetProp(root, "name");
    char* texture_path = xmlGetProp(root, "src");
    sprite.spritesheet = LoadTextureFromPhysFS(texture_path);

    #if HAL_DEBUG_FEATURES
    printf ("name: %s, texture path: %s\n", sprite.name, texture_path);
    #endif

    free(texture_path);

    // set flags
    sprite.tick = 0;
    sprite.frame_counter = 0;
    sprite.total_animations = 0;
    sprite.current_animation = 0;

    // count amount of animations
    sprite.total_animations = CountChildElementsOfType(root, "anim");

    if (sprite.total_animations > 0)
    {
        sprite.animations = malloc(sprite.total_animations * sizeof(Animation));
        #if HAL_DEBUG_FEATURES
        printf("allocating for %i animation(s)\n", sprite.total_animations);
        #endif
    }

    // begin parsing
    xmlNode* anim_element = root->children;
    for (int each_anim = 0; each_anim < sprite.total_animations;)
    {
        if (anim_element->type == XML_ELEMENT_NODE)
        {
            // get name
            sprite.animations[each_anim].name = xmlGetProp(anim_element, "name");

            // get speed
            char* speed_str = xmlGetProp(anim_element, "speed");
            sprite.animations[each_anim].speed = QuickStoi(speed_str);
            free(speed_str);

            #if HAL_DEBUG_FEATURES
            printf("<=== loading animation \"%s\" with speed %i ===>\n", sprite.animations[each_anim].name, sprite.animations[each_anim].speed);
            #endif

            // count frames for allocation
            sprite.animations[each_anim].frame_count = CountChildElements(anim_element);

            // allocate 
            if (sprite.animations[each_anim].frame_count > 0)
            {
                sprite.animations[each_anim].sources = malloc(sprite.animations[each_anim].frame_count
                                                                                     * sizeof(Rectangle));
                #if HAL_DEBUG_FEATURES
                printf("allocating for %i frame(s)\n", sprite.animations[each_anim].frame_count);
                #endif
            }

            // parse frames
            xmlNode* frame_element = anim_element->children;
            for (int each_frame = 0; each_frame <  sprite.animations[each_anim].frame_count;)
            {
                if (frame_element->type == XML_ELEMENT_NODE)
                {
                    #if HAL_DEBUG_FEATURES
                    printf("loading frame %i\n", each_frame);
                    #endif
                    
                    char* rectangle_str = xmlGetProp(frame_element, "src");
                    sprite.animations[each_anim].sources[each_frame] = StringToRectangle(rectangle_str);
                    free(rectangle_str);

                    #if HAL_DEBUG_FEATURES
                    printf("(Rectangle) { %f, %f, %f, %f }\n",
                        sprite.animations[each_anim].sources[each_frame].x, 
                        sprite.animations[each_anim].sources[each_frame].y, 
                        sprite.animations[each_anim].sources[each_frame].width, 
                        sprite.animations[each_anim].sources[each_frame].height);
                    #endif
                    each_frame++;
                }

                frame_element = frame_element->next;
            }
            each_anim++;
        }
        anim_element = anim_element->next;
    }

    // close parser
    xmlFreeDoc(document);
    xmlCleanupParser();

    #if !HAL_DEBUG_FEATURES
    printf("loaded sprite %s, containing %i animations\n", sprite.name, sprite.total_animations);
    #else
    float elapsed = (clock() - start);

    printf("loaded sprite %s, containing %i animations in %f seconds\n", sprite.name, sprite.total_animations, elapsed / CLOCKS_PER_SEC);
    printf("-------> END SPRITE LOAD <-------\n");
    #endif
    
    return sprite;
}

void UnloadSprite(Sprite* sprite)
{
    free(sprite->name);
    UnloadTexture(sprite->spritesheet);
    for (int i = 0; i < sprite->total_animations; i++)
    {
        free(sprite->animations[i].name);
        free(sprite->animations[i].sources);
    }

    free(sprite->animations);
}

void SpriteSwitchToString(Sprite* sprite, const char* anim_id)
{
    for (int i = 0; i < sprite->total_animations; i++)
    {
        if (strcmp(anim_id, sprite->animations[i].name) == 0)
        {
            SpriteBindIndex(sprite, i);
        }
    }
}

void SpriteBindIndex(Sprite* sprite, int index)
{
    sprite->current_animation = index;
    // sprite->frame_counter = 0;
    // sprite->tick = 0;
}

void UpdateSprite(Sprite* sprite)
{
    sprite->tick++;

    if (sprite->tick % sprite->animations[sprite->current_animation].speed == 0) sprite->frame_counter++;

    if (sprite->frame_counter > sprite->animations[sprite->current_animation].frame_count - 1)
    {
        sprite->frame_counter = 0;
    }
}

void DrawSprite(Sprite* sprite, Vector2 where)
{
    // DrawTextureRec(sprite->spritesheet, 
    //             sprite->animations[sprite->current_animation].sources[sprite->frame_counter],
    //             where,
    //             WHITE);

    DrawTexturePro(sprite->spritesheet,
                sprite->animations[sprite->current_animation].sources[sprite->frame_counter],
                (Rectangle) { where.x, where.y, 
                sprite->animations[sprite->current_animation].sources[sprite->frame_counter].width, sprite->animations[sprite->current_animation].sources[sprite->frame_counter].height },
                (Vector2) { sprite->animations[sprite->current_animation].sources[sprite->frame_counter].width / 2, sprite->animations[sprite->current_animation].sources[sprite->frame_counter].height / 2 },
                0.0f,
                WHITE);
}