#include <stdbool.h>

#ifndef SPRITE_H
#define SPRITE_H

typedef struct {
    float x;
    float y;
    float distance;
    float angle;
    bool visible;
    int texture;
} sprite_t;

void renderSprites();
void renderMapSprites();

#endif