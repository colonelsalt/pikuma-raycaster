#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "defs.h"
#include "texture.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup()
{
    loadWallTextures();
}

void processInput()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
        case SDL_QUIT:
            isGameRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isGameRunning = false;
            if (event.key.keysym.sym == SDLK_w)
                player.walkDirection = 1;
            if (event.key.keysym.sym == SDLK_s)
                player.walkDirection = -1;
            if (event.key.keysym.sym == SDLK_d)
                player.turnDirection = 1;
            if (event.key.keysym.sym == SDLK_a)
                player.turnDirection = -1;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_w)
                player.walkDirection = 0;
            if (event.key.keysym.sym == SDLK_s)
                player.walkDirection = 0;
            if (event.key.keysym.sym == SDLK_d)
                player.turnDirection = 0;
            if (event.key.keysym.sym == SDLK_a)
                player.turnDirection = 0;
            break;
    }
}

void update()
{
    //int msToWait = TARGET_FRAME_TIME - (SDL_GetTicks() - ticksLastFrame);
    //if (msToWait > 0 && msToWait <= TARGET_FRAME_TIME)
    //    SDL_Delay(msToWait);

    float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

    ticksLastFrame = SDL_GetTicks();

    movePlayer(deltaTime);
    castAllRays();
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
            
            uint32_t texelColor = wallTextures[texNum].textureBuffer[texOffsetY * texWidth + texOffsetX];
            setPixel(x, y, texelColor);
        }

        // Render floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
            setPixel(x, y, 0xFF707070);
    }
}

void render()
{
    clearColorBuffer(0xFF000000);
    generateWallProjection();
    
    // render mini-map stuff
    renderMap();
    renderPlayer();
    // renderRays();
    
    renderColorBuffer();
}

void releaseResources(void)
{
    freeWallTextures();
    destroyWindow();
}

int main()
{
    isGameRunning = initWindow();

    setup();
    while(isGameRunning)
    {
        processInput();
        update();
        render();
    }

    releaseResources();

    return 0;
}