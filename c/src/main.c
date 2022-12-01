#include <stdio.h>
#include <SDL2/SDL.h>

#include "constants.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int isGameRunning = FALSE;
int ticksLastFrame = 0;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player
{
    float x;
    float y;
    float width;
    float height;
    int turnDirection; // -1 for left; +1 for right
    int walkDirection; // -1 for back; +1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player;

void setup()
{
    player.x = WINDOW_WIDTH / 2;
    player.y = WINDOW_HEIGHT / 2;
    player.width = 1;
    player.height = 1;
    player.turnDirection = 0;
    player.walkDirection = 0;
    player.rotationAngle = PI / 2.0f;
    player.walkSpeed = 100;
    player.turnSpeed = 45 * (PI / 180.0f);
}

int gridWallAt(float x, float y)
{
    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
        return TRUE;

    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    return map[tileY][tileX] != 0;
}

void movePlayer(float deltaTime)
{
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
    
    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

    float newX = player.x + cos(player.rotationAngle) * moveStep;
    float newY = player.y + sin(player.rotationAngle) * moveStep;

    if (!gridWallAt(newX, newY))
    {
        player.x = newX;
        player.y = newY;
    }

}

void renderPlayer()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = {
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR
    };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderDrawLine(
        renderer,
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        (player.x + cos(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR,
        (player.y + sin(player.rotationAngle) * 40) * MINIMAP_SCALE_FACTOR
    );
}

void renderMap()
{
    for (int i = 0; i < MAP_NUM_ROWS; i++)
    {
        for (int j = 0; j < MAP_NUM_COLS; j++)
        {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            int color = map[i][j] != 0 ? 255 : 0;

            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_Rect tileRect = {
                tileX * MINIMAP_SCALE_FACTOR,
                tileY * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR
            };
            SDL_RenderFillRect(renderer, &tileRect);

        }
    }
}

void processInput()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
        case SDL_QUIT:
            isGameRunning = FALSE;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                isGameRunning = FALSE;
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

int initWindow()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initialising SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow(NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return TRUE;
}

void destroyWindow()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update()
{
    //int msToWait = TARGET_FRAME_TIME - (SDL_GetTicks() - ticksLastFrame);
    //if (msToWait > 0 && msToWait <= TARGET_FRAME_TIME)
    //    SDL_Delay(msToWait);

    float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

    ticksLastFrame = SDL_GetTicks();

    movePlayer(deltaTime);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderMap();
    //renderRays();
    renderPlayer();

    SDL_RenderPresent(renderer);
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

    destroyWindow();

    return 0;
}