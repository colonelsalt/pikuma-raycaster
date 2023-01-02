#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

#include "defs.h"
#include "upng.h"

extern upng_t* textures[NUM_TEXTURES];

void loadTextures();
void freeTextures();

#endif