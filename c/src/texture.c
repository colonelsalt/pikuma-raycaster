#include "texture.h"

#include <stdio.h>

upng_t* textures[NUM_TEXTURES];

static const char* textureFilenames[NUM_TEXTURES] = {
    "./textures/redbrick.png",          // [0]
    "./textures/purplestone.png",       // [1]
    "./textures/mossystone.png",        // [2]
    "./textures/graystone.png",         // [3]
    "./textures/colorstone.png",        // [4]   
    "./textures/bluestone.png",         // [5]
    "./textures/wood.png",              // [6]  
    "./textures/eagle.png",             // [7]
    "./textures/pikuma.png",            // [8]
    
    // Sprites
    "./textures/barrel.png",            // [9]
    "./textures/light.png",             // [10]
    "./textures/table.png",             // [11]
    "./textures/guard.png",             // [12]
    "./textures/armor.png"              // [13]
};

void loadTextures()
{
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        upng_t* upng = upng_new_from_file(textureFilenames[i]);
        if (upng)
        {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK)
            {
                textures[i] = upng;
            }
            else
            {
                printf("Error decoding texture %s\n", textureFilenames[i]);
            }
        }
        else 
        {
            printf("Error loading texture %s\n", textureFilenames[i]);
        }
    }
}

void freeTextures()
{
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        upng_free(textures[i]);
    }
}