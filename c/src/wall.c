#include "wall.h"

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

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        if (wallTopPixel < 0)
            wallTopPixel = 0;
        
        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        if (wallBottomPixel > WINDOW_HEIGHT)
            wallBottomPixel = WINDOW_HEIGHT;
        
        // Render ceiling
        for (int y = 0; y < wallTopPixel; y++)
            setPixel(x, y, 0xFF383838);

        int texOffsetX;
        if (rays[x].wasHitVertical)
            texOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
        else
            texOffsetX = (int)rays[x].wallHitX % TILE_SIZE;

        int texNum = rays[x].wallHitContent - 1;
        int texWidth = wallTextures[texNum].width;
        int texHeight = wallTextures[texNum].height;

        // Render wall strip from top to bottom
        for (int y = wallTopPixel; y < wallBottomPixel; y++)
        {
            int distFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int texOffsetY = distFromTop * ((float)texHeight / wallStripHeight);
            
            color_t texelColor = wallTextures[texNum].textureBuffer[texOffsetY * texWidth + texOffsetX];
            if (rays[x].wasHitVertical)
                darkenColor(&texelColor, 0.7f);
            setPixel(x, y, texelColor);
        }

        // Render floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
            setPixel(x, y, 0xFF707070);
    }
}