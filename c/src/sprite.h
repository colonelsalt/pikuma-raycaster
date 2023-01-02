#ifndef SPRITE_H
#define SPRITE_H

typedef struct {
    float x;
    float y;
    float distance;
    float angle;
    int texture;
} sprite_t;

void renderSprites();
void renderMapSprites();

#endif