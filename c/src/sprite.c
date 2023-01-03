#include "sprite.h"

#include "graphics.h"
#include "player.h"
#include "texture.h"
#include "defs.h"
#include "utils.h"
#include "ray.h"

#define NUM_SPRITES 6

static sprite_t sprites[NUM_SPRITES] = {
    { .x = 640, .y = 630, .texture = 9 }, // barrel
    { .x = 660, .y = 690, .texture = 9 }, // barrel
    { .x = 250, .y = 600, .texture = 11 }, // table
    { .x = 250, .y = 600, .texture = 10 }, // light
    { .x = 300, .y = 400, .texture = 12 }, // guard
    { .x = 90,  .y = 100, .texture = 13 } // armor
};

void renderMapSprites()
{
    for (int i = 0; i < NUM_SPRITES; i++)
    {
        color_t spriteColor = sprites[i].visible ? 0xFF00FFFF : 0xFF444444;
        drawScaledRect(sprites[i].x, sprites[i].y, 2, 2, spriteColor);
    }
}

void bubbleSort(sprite_t* s, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (s[i].distance < s[j].distance)
            {
                sprite_t temp = s[i];
                s[i] = s[j];
                s[j] = temp;
            }
        }
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

        const float EPSILON = 0.2f;
        if (angle < (FOV_ANGLE / 2) + EPSILON)
        {
            sprites[i].angle = angle;
            sprites[i].distance = distance(sprites[i].x, sprites[i].y, player.x, player.y);
            sprites[i].visible = true;
            visibleSprites[numVisibleSprites] = sprites[i];
            numVisibleSprites++;
        }
        else
        {
            sprites[i].visible = false;
        }
    }

    bubbleSort(visibleSprites, numVisibleSprites);

    // Render visible sprites
    for (int i = 0; i < numVisibleSprites; i++)
    {
        sprite_t sprite = visibleSprites[i];

        float perpDist = sprite.distance * cos(sprite.angle);

        float spriteHeight = (TILE_SIZE / perpDist) * DIST_PROJ_PLANE;
        float spriteWidth = spriteHeight;

        // Sprite top y
        float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
        if (spriteTopY < 0)
            spriteTopY = 0;
        
        float spriteBottomY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
        if (spriteBottomY > WINDOW_HEIGHT)
            spriteBottomY = WINDOW_HEIGHT;

        float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
        float spriteScreenPosX = tan(spriteAngle) * DIST_PROJ_PLANE;

        float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteWidth / 2);
        float spriteRightX = spriteLeftX + spriteWidth;

        int texWidth = upng_get_width(textures[sprite.texture]);
        int texHeight = upng_get_height(textures[sprite.texture]);

        for (int y = spriteTopY; y < spriteBottomY; y++)
        {
            int distFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
            int texOffsetY = distFromTop * (texHeight / spriteHeight);
            for (int x = spriteLeftX; x < spriteRightX; x++)
            {
                float texelWidth = texWidth / spriteWidth;
                int texOffsetX = (x - spriteLeftX) * texelWidth;
                if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT)
                {
                    color_t* texBuffer = (color_t*)upng_get_buffer(textures[sprite.texture]);
                    color_t texelColor = texBuffer[(texWidth * texOffsetY) + texOffsetX];
                    
                    if (sprite.distance < rays[x].distance && texelColor != 0xFFFF00FF)
                        setPixel(x, y, texelColor);
                }
            }
        }
    }
}