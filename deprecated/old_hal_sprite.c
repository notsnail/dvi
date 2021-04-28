#include "hal_sprite.h"

#include <time.h>
#include "ext/asm-xml.h"

#define DEBUG_FEATURES 1

#pragma Specification
#define SPRITE_XML_ANIM_ID 1

static const char* XML_Schema = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
                                "<schema>\n" \
                                "   <document name=\"sprite\">\n" \
                                "       <attribute name=\"name\"/>\n" \
                                "       <attribute name=\"src\"/>\n" \
                                "       <collection name=\"anim\" id=\"1\">\n" \
                                "           <attribute name=\"name\"/>\n" \
                                "           <attribute name=\"speed\"/>\n" \
                                "           <collection name=\"frame\">\n" \
                                "               <attribute name=\"src\"/>\n" \
                                "           </collection>\n" \
                                "       </collection>\n" \
                                "   </document>\n" \
                                "</schema>"  
;
#pragma End Specification

#include <stdio.h>
#include <string.h>
#include <malloc.h>

static void ReportError(const char* why)
{
    printf("sprite xml error: %s\n", why);
}

static char* AttributeToString(AXAttribute *attr, int *size)
{
    const char *start = attr->begin;
    const char *limit = attr->limit;
    size_t tsize = limit - start;

    *size = tsize;

    char *buffer = malloc(size);

    memcpy(buffer, start, tsize);
    buffer[tsize] = 0;

    return buffer;
}

HalcyonSprite HalcyonLoadSprite(const char* filename)
{
    #if DEBUG_FEATURES
    printf("-------> START SPRITE LOAD <-------\n");
    clock_t start = clock();
    #endif

    // initialize parser
    ax_initialize(malloc, free);
    
    AXClassContext class_context;
    if (ax_initializeClassParser(&class_context) != RC_OK)
    {
        ReportError("failed to intialize context!");
        return (HalcyonSprite) { 0 };
    }

    AXElementClass* schema_class = ax_classFromString(XML_Schema, &class_context);
    if (schema_class == NULL)
    {
        ReportError("failed to open schema class!");
        return (HalcyonSprite) { 0 };
    }

    AXParseContext parse_context;
    if (ax_initializeParser(&parse_context, 16 * 1024 * 1024) != RC_OK)
    {
        ReportError("bruh! parse context creation failed.");
        return (HalcyonSprite) { 0 };
    }

    // load file into string
    char* sprite_xml = LoadFileText(filename);
    if (sprite_xml == NULL)
    {
        ReportError(TextFormat("failed to open sprite file! (%s)", filename));
        return (HalcyonSprite) { 0 };
    }

    // start parsing
    AXElement* root = ax_parse(&parse_context, sprite_xml, schema_class, 1); // "sprite" element
    if (root == NULL)
    {
        ReportError("failed to find root element!");
        return (HalcyonSprite) { 0 };
    }

    int size; // reused for AttributeToString functions

    // set up initial variables
    HalcyonSprite returnValue;

    returnValue.name = AttributeToString(&root->attributes[0], &size);
    char* texturePath = AttributeToString(&root->attributes[1], &size);    
    returnValue.Spritesheet = LoadTexture(texturePath);
    free(texturePath);

    returnValue.tick = 0;
    returnValue.frameCounter = 0;
    returnValue.TotalAnimations = 0;
    returnValue.currentAnimation = 0;

    // parse loop
    AXElement* loopElement = root->firstChild;

    // count amount of animations
    while (loopElement)
    {
        loopElement = loopElement->nextSibling;
        returnValue.TotalAnimations++;
    }

    if (returnValue.TotalAnimations > 0)
    {
        returnValue.Animations = malloc(returnValue.TotalAnimations * sizeof(HalcyonAnimation));
        #if DEBUG_FEATURES
        printf("allocating for %i animation(s)\n", returnValue.TotalAnimations);
        #endif
    }

    loopElement = root->firstChild;
    int animationCounter = 0;

    while (loopElement && animationCounter < returnValue.TotalAnimations)
    {
        if (loopElement->id == SPRITE_XML_ANIM_ID)
        {
            AXElement* frame = loopElement->firstChild;
            returnValue.Animations[animationCounter].name = AttributeToString(&loopElement->attributes[0], &size);
            
            char* speedStr = AttributeToString(&loopElement->attributes[1], &size);
            int speed;
            sscanf(speedStr, "%i", &speed);
            free(speedStr);
    
            returnValue.Animations[animationCounter].Speed = speed;
            returnValue.Animations[animationCounter].FrameCount = 0;
            
            // count frames for allocation
            while (frame)
            {
                frame = frame->nextSibling;
                returnValue.Animations[animationCounter].FrameCount++;
            }

            if (returnValue.Animations[animationCounter].FrameCount > 0)
            {
                returnValue.Animations[animationCounter].Sources = malloc(returnValue.Animations[animationCounter].FrameCount
                                                                                     * sizeof(Rectangle));
                #if DEBUG_FEATURES
                printf("allocating for %i frame(s)\n", returnValue.Animations[animationCounter].FrameCount);
                #endif
            }

            int frameCounter = 0;
            frame = loopElement->firstChild;
            while (frame && frameCounter < returnValue.Animations[animationCounter].FrameCount)
            {
                #if DEBUG_FEATURES
                printf("loading frame %i\n", frameCounter);
                #endif
                
                char* srcStr = AttributeToString(&frame->attributes[0], &size);
                int x, y, width, height;
                sscanf(srcStr, "%i %i %i %i", &x, &y, &width, &height);
                free(srcStr);

                #if DEBUG_FEATURES
                printf("(Rectangle) { %i, %i, %i, %i }\n", x, y, width, height);
                #endif

                returnValue.Animations[animationCounter].Sources[frameCounter] = (Rectangle) { x, y, width, height };

                frame = frame->nextSibling;
                frameCounter++;
            }
            animationCounter++;
        }

        loopElement = loopElement->nextSibling;
    }

    // close parser
    ax_releaseParser(&parse_context);
    ax_releaseClassParser(&class_context);

    // close file, release other sources
    free(sprite_xml);

    #if !DEBUG_FEATURES
    printf("loaded sprite %s, containing %i animations\n", returnValue.name, returnValue.TotalAnimations);
    #else
    float elapsed = (clock() - start);

    printf("loaded sprite %s, containing %i animations in %f milliseconds\n", returnValue.name, returnValue.TotalAnimations, elapsed);
    printf("-------> END SPRITE LOAD <-------\n");
    #endif
    
    return returnValue;
}

void HalcyonUnloadSprite(HalcyonSprite* sprite)
{
    free(sprite->name);
    UnloadTexture(sprite->Spritesheet);
    for (int i = 0; i < sprite->TotalAnimations; i++)
    {
        free(sprite->Animations[i].name);
        free(sprite->Animations[i].Sources);
    }

    free(sprite->Animations);
}

void HalcyonSwitchSpriteToString(HalcyonSprite* sprite, const char* anim_id)
{
    for (int i = 0; i < sprite->TotalAnimations; i++)
    {
        if (strcmp(anim_id, sprite->Animations[i].name) == 0)
        {
            HalcyonSwitchSpriteToIndex(sprite, i);
        }
    }
}

void HalcyonSwitchSpriteToIndex(HalcyonSprite* sprite, int index)
{
    sprite->currentAnimation = index;
    // sprite->frameCounter = 0;
    // sprite->tick = 0;
}

void HalcyonUpdateSprite(HalcyonSprite* sprite)
{
    sprite->tick++;

    if (sprite->tick % sprite->Animations[sprite->currentAnimation].Speed == 0) sprite->frameCounter++;

    if (sprite->frameCounter > sprite->Animations[sprite->currentAnimation].FrameCount - 1)
    {
        sprite->frameCounter = 0;
    }
}

void HalcyonDrawSprite(HalcyonSprite* sprite, Vector2 where)
{
    DrawTextureRec(sprite->Spritesheet, 
                sprite->Animations[sprite->currentAnimation].Sources[sprite->frameCounter],
                where,
                WHITE);
}