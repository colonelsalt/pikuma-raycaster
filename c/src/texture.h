#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#include "defs.h"
#include "upng.h"

typedef struct
{
    upng_t* upngTexture;
    int width;
    int height;
    uint32_t* textureBuffer;
} texture_t;

extern texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();
void freeWallTextures();

#endif