#include "graphics.h"

#include <SDL2/SDL.h>

#include "defs.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static color_t* colorBuffer = NULL;
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

    colorBuffer = malloc(sizeof(color_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

void setPixel(int x, int y, color_t color)
{
    colorBuffer[y * WINDOW_WIDTH + x] = color;
}

void drawRect(int x, int y, int width, int height, color_t color)
{
    for (int i = y; i <= (y + height); i++)
        for (int j = x; j <= (x + width); j++)
            setPixel(j, i, color);
}

int max(int x, int y)
{
    return x > y ? x : y;
}

void drawScaledLine(int x0, int y0, int x1, int y1, color_t color)
{
    drawLine(x0 * MINIMAP_SCALE_FACTOR,
             y0 * MINIMAP_SCALE_FACTOR,
             x1 * MINIMAP_SCALE_FACTOR,
             y1 * MINIMAP_SCALE_FACTOR,
             color);
}

void drawLine(int x0, int y0, int x1, int y1, color_t color)
{
    int deltaX = x1 - x0;
    int deltaY = y1 - y0;

    int sideLength = max(abs(deltaX), abs(deltaY));

    float xInc = deltaX / (float)sideLength;
    float yInc = deltaY / (float)sideLength;

    float currX = x0;
    float currY = y0;
    for (int i = 0; i < sideLength; i++)
    {
        setPixel(round(currX), round(currY), color);
        currX += xInc;
        currY += yInc;
    }
}

void drawScaledRect(float x, float y, float width, float height, color_t color)
{
    drawRect(x * MINIMAP_SCALE_FACTOR,
             y * MINIMAP_SCALE_FACTOR,
             width * MINIMAP_SCALE_FACTOR,
             height * MINIMAP_SCALE_FACTOR,
             color);
}

void destroyWindow(void)
{
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clearColorBuffer(color_t color)
{
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
        colorBuffer[i] = color;
}

void renderColorBuffer(void)
{
    SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, WINDOW_WIDTH * sizeof(color_t));
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}