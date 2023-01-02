#include "wall.h"

#include <stdio.h>

#include "ray.h"
#include "player.h"
#include "graphics.h"
#include "texture.h"

static void darkenColor(color_t* color, float intensity)
{
    color_t a = (*color & 0xFF000000);
    color_t r = (*color & 0x00FF0000) * intensity;
    color_t g = (*color & 0x0000FF00) * intensity;
    color_t b = (*color & 0x000000FF) * intensity;

    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

void generateWallProjection()
{
    for (int x = 0; x < NUM_RAYS; x++)
    {
        float perpDist = rays[x].distance * cos(rays[x].angle - player.rotationAngle);
        
        int wallStripHeight = (int)((TILE_SIZE / perpDist) * DIST_PROJ_PLANE);

        int wallTopY = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        if (wallTopY < 0)
            wallTopY = 0;
        
        int wallBottomY = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        if (wallBottomY > WINDOW_HEIGHT)
            wallBottomY = WINDOW_HEIGHT;
        
        // Render ceiling
        for (int y = 0; y < wallTopY; y++)
            setPixel(x, y, 0xFF383838);

        int texOffsetX;
        if (rays[x].wasHitVertical)
            texOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
        else
            texOffsetX = (int)rays[x].wallHitX % TILE_SIZE;

        int texNum = rays[x].wallHitContent - 1;
        int texWidth = upng_get_width(textures[texNum]);
        int texHeight = upng_get_height(textures[texNum]);

        // Render wall strip from top to bottom
        for (int y = wallTopY; y < wallBottomY; y++)
        {
            int distFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int texOffsetY = distFromTop * ((float)texHeight / wallStripHeight);
            
            color_t* texBuffer = (color_t*) upng_get_buffer(textures[texNum]);
            color_t texelColor = texBuffer[texOffsetY * texWidth + texOffsetX];
            if (rays[x].wasHitVertical)
                darkenColor(&texelColor, 0.7f);
            setPixel(x, y, texelColor);
        }

        // Render floor
        for (int y = wallBottomY; y < WINDOW_HEIGHT; y++)
            setPixel(x, y, 0xFF707070);
    }
}