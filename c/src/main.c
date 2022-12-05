#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "constants.h"
#include "textures.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int isGameRunning = FALSE;
int ticksLastFrame = 0;

uint32_t* colorBuffer = NULL;

SDL_Texture* colorBufferTexture = NULL;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
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

struct Ray
{
    float angle;
    float wallHitX, wallHitY;
    float distance;
    int wasHitVertical;
    int facingUp;
    int facingDown;
    int facingRight;
    int facingLeft;
    int wallHitContent;
} rays[NUM_RAYS];

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

    colorBuffer = malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    loadWallTextures();
}

int getGridContent(float x, float y)
{
    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
        return 1;

    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;

    return map[tileY][tileX];
}

void movePlayer(float deltaTime)
{
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
    
    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

    float newX = player.x + cos(player.rotationAngle) * moveStep;
    float newY = player.y + sin(player.rotationAngle) * moveStep;

    if (getGridContent(newX, newY) == 0)
    {
        player.x = newX;
        player.y = newY;
    }

}

float normalizeAngle(float angle)
{
    angle = remainder(angle, TWO_PI);
    if (angle < 0)
        angle = TWO_PI + angle;
    return angle;
}

float distance(float px, float py, float qx, float qy)
{
    return sqrt((qx - px) * (qx - px) + (qy - py) * (qy - py));
}

void castRay(float rayAngle, int rayId)
{
    struct Ray* ray = &rays[rayId];
    ray->angle = normalizeAngle(rayAngle);

    ray->facingDown = ray->angle > 0 && ray->angle < PI;
    ray->facingUp = !ray->facingDown;
    ray->facingRight = ray->angle < PI * 0.5 || ray->angle > PI * 1.5;
    ray->facingLeft = !ray->facingRight;

    // Horizontal intersections
    float yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    if (ray->facingDown)
        yintercept += TILE_SIZE;
    
    float xintercept = player.x + ((yintercept - player.y) / tan(ray->angle));
    float ystep = TILE_SIZE;
    if (ray->facingUp)
        ystep *= -1;

    float xstep = TILE_SIZE / tan(ray->angle);
    if ((ray->facingLeft && xstep > 0) || (ray->facingRight && xstep < 0))
        xstep *= -1;
    
    float nextIntersectionX = xintercept;
    float nextIntersectionY = yintercept;
    
    int horizontalWallHitFound = FALSE;
    float horizontalWallHitX = 0;
    float horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    while (nextIntersectionX >= 0 && nextIntersectionX <= WINDOW_WIDTH
        && nextIntersectionY >= 0 && nextIntersectionY <= WINDOW_HEIGHT)
    {
        int gridContent = getGridContent(nextIntersectionX, nextIntersectionY - (ray->facingDown ? 0 : 1));
        if (gridContent != 0)
        {
            // Hit a wall
            horizontalWallHitFound = TRUE;
            horizontalWallHitX = nextIntersectionX;
            horizontalWallHitY = nextIntersectionY;
            horizontalWallContent = gridContent;
            break;
        }
        else
        {
            nextIntersectionX += xstep;
            nextIntersectionY += ystep;
        }
    }

    // Vertical intersections
    xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    if (ray->facingRight)
        xintercept += TILE_SIZE;
    
    yintercept = player.y + ((xintercept - player.x) * tan(ray->angle));
    xstep = TILE_SIZE;
    if (ray->facingLeft)
        xstep *= -1;

    ystep = TILE_SIZE * tan(ray->angle);
    if ((ray->facingUp && ystep > 0) || (ray->facingDown && ystep < 0))
        ystep *= -1;
    
    nextIntersectionX = xintercept;
    nextIntersectionY = yintercept;
    
    int verticalWallHitFound = FALSE;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallContent = 0;

    while (nextIntersectionX >= 0 && nextIntersectionX <= WINDOW_WIDTH
        && nextIntersectionY >= 0 && nextIntersectionY <= WINDOW_HEIGHT)
    {
        int gridContent = getGridContent(nextIntersectionX - (ray->facingRight ? 0 : 1), nextIntersectionY);
        if (gridContent != 0)
        {
            // Hit a wall
            verticalWallHitFound = TRUE;
            verticalWallHitX = nextIntersectionX;
            verticalWallHitY = nextIntersectionY;
            verticalWallContent = gridContent;
            break;
        }
        else
        {
            nextIntersectionX += xstep;
            nextIntersectionY += ystep;
        }
    }

    float xDist = horizontalWallHitFound ? distance(player.x, player.y, horizontalWallHitX, horizontalWallHitY) : FLT_MAX;
    float yDist = verticalWallHitFound ? distance(player.x, player.y, verticalWallHitX, verticalWallHitY) : FLT_MAX;

    if (yDist < xDist)
    {
        ray->distance = yDist;
        ray->wallHitX = verticalWallHitX;
        ray->wallHitY = verticalWallHitY;
        ray->wallHitContent = verticalWallContent;
        ray->wasHitVertical = TRUE;
    }
    else
    {
        ray->distance = xDist;
        ray->wallHitX = horizontalWallHitX;
        ray->wallHitY = horizontalWallHitY;
        ray->wallHitContent = horizontalWallContent;
        ray->wasHitVertical = FALSE;
    }
}

void castAllRays()
{
    for (int i = 0; i < NUM_RAYS; i++)
    {
        float rayAngle = player.rotationAngle + atan((i - NUM_RAYS / 2) / DIST_PROJ_PLANE);
        castRay(rayAngle, i);
    }
}

void drawScaledRect(float x, float y, float width, float height)
{
    SDL_Rect rect = {
        x * MINIMAP_SCALE_FACTOR,
        y * MINIMAP_SCALE_FACTOR,
        width * MINIMAP_SCALE_FACTOR,
        height * MINIMAP_SCALE_FACTOR
    };
    SDL_RenderFillRect(renderer, &rect);
}

void drawScaledLine(float x1, float y1, float x2, float y2)
{
    SDL_RenderDrawLine(
        renderer,
        x1 * MINIMAP_SCALE_FACTOR,
        y1 * MINIMAP_SCALE_FACTOR,
        x2 * MINIMAP_SCALE_FACTOR,
        y2 * MINIMAP_SCALE_FACTOR
    );
}

void renderPlayer()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawScaledRect(player.x, player.y, player.width, player.height);
    
    drawScaledLine(
        player.x,
        player.y,
        player.x + cos(player.rotationAngle) * 40,
        player.y + sin(player.rotationAngle) * 40
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
            drawScaledRect(tileX, tileY, TILE_SIZE, TILE_SIZE);
        }
    }
}

void renderRays()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++)
    {
        drawScaledLine(player.x, player.y, rays[i].wallHitX, rays[i].wallHitY);
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
    freeWallTextures();
    free(colorBuffer);
    SDL_DestroyTexture(colorBufferTexture);
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
    castAllRays();
}

void generateWallProjection()
{
    for (int i = 0; i < NUM_RAYS; i++)
    {
        float perpDist = rays[i].distance * cos(rays[i].angle - player.rotationAngle);
        
        int wallStripHeight = (int)((TILE_SIZE / perpDist) * DIST_PROJ_PLANE);

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        if (wallTopPixel < 0)
            wallTopPixel = 0;
        
        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        if (wallBottomPixel > WINDOW_HEIGHT)
            wallBottomPixel = WINDOW_HEIGHT;
        
        // Render ceiling
        for (int y = 0; y < wallTopPixel; y++)
            colorBuffer[y * WINDOW_WIDTH + i] = 0xFF383838;

        int texOffsetX;
        if (rays[i].wasHitVertical)
            texOffsetX = (int)rays[i].wallHitY % TILE_SIZE;
        else
            texOffsetX = (int)rays[i].wallHitX % TILE_SIZE;

        int texNum = rays[i].wallHitContent - 1;
        int texWidth = wallTextures[texNum].width;
        int texHeight = wallTextures[texNum].height;

        // Render wall strip from top to bottom
        for (int y = wallTopPixel; y < wallBottomPixel; y++)
        {
            int distFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
            int texOffsetY = distFromTop * ((float)texHeight / wallStripHeight);
            
            uint32_t texelColor = wallTextures[texNum].textureBuffer[texOffsetY * texWidth + texOffsetX];

            colorBuffer[y * WINDOW_WIDTH + i] = texelColor;
        }

        // Render floor
        for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++)
            colorBuffer[y * WINDOW_WIDTH + i] = 0xFF707070;
    }
}

void clearColorBuffer(uint32_t color)
{
    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        for (int x = 0; x < WINDOW_WIDTH; x++)
        {
            colorBuffer[y * WINDOW_WIDTH + x] = color;
        }
    }
}

void renderColorBuffer()
{
    SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, WINDOW_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    generateWallProjection();

    renderColorBuffer();
    clearColorBuffer(0xFF000000);

    // render mini-map stuff
    renderMap();
    renderRays();
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