#include "sprite.h"

#include "graphics.h"
#include "player.h"
#include "texture.h"
#include "defs.h"
#include "utils.h"

#define NUM_SPRITES 2

static sprite_t sprites[NUM_SPRITES] = {
    { .x = 640, .y = 630, .texture = 9 } // barrel
};

void renderMapSprites()
{
    for (int i = 0; i < NUM_SPRITES; i++)
    {
        color_t spriteColor = sprites[i].visible ? 0xFF00FFFF : 0xFF444444;
        drawScaledRect(sprites[i].x, sprites[i].y, 2, 2, spriteColor);
    }
}

void renderSprites()
{
    sprite_t visibleSprites[NUM_SPRITES];
    int numVisibleSprites = 0;

    for (int i = 0; i < NUM_SPRITES; i++)
    {
        // Angle between sprite and player
        float angle = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

        // Make sure angle difference is in range [0, PI]
        if (angle > PI)
            angle -= TWO_PI;
        if (angle < -PI)
            angle += TWO_PI;
        angle = fabs(angle);

        if (angle < FOV_ANGLE / 2)
        {
            sprites[i].visible = true;
            sprites[i].angle = angle;
            sprites[i].distance = distance(sprites[i].x, sprites[i].y, player.x, player.y);
            visibleSprites[numVisibleSprites] = sprites[i];
            numVisibleSprites++;
        }
        else
        {
            sprites[i].visible = false;
        }
    }

    // Render visible sprites
    for (int i = 0; i < numVisibleSprites; i++)
    {

    }
}