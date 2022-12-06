#include "graphics.h"

#include <SDL2/SDL.h>

#include "defs.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* colorBuffer = NULL;
static SDL_Texture* colorBufferTexture = NULL;

bool initWindow(void)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initialising SDL.\n");
        return false;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    int fullScreenWidth = displayMode.w;
    int fullScreenHeight = displayMode.h;
    window = SDL_CreateWindow(NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        fullScreenWidth,
        fullScreenHeight,
        SDL_WINDOW_BORDERLESS);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    colorBuffer = malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

void setPixel(int x, int y, uint32_t color)
{
    colorBuffer[y * WINDOW_WIDTH + x] = color;
}

void destroyWindow(void)
{
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clearColorBuffer(uint32_t color)
{
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
        colorBuffer[i] = color;
}

void renderColorBuffer(void)
{
    SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, WINDOW_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}