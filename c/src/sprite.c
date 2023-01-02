#include "sprite.h"

#include "graphics.h"
#include "player.h"
#include "texture.h"
#include "defs.h"

#define NUM_SPRITES 1

static sprite_t sprites[NUM_SPRITES] = {
    { .x = 640, .y = 630, .texture = 9 } // barrel
};

void renderMapSprites()
{
    for (int i = 0; i < NUM_SPRITES; i++)
    {
        drawScaledRect(sprites[i].x, sprites[i].y, 2, 2, 0xFF00FFFF);
    }
}

void renderSprites()
{

}