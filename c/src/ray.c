#include "ray.h"

#include <float.h>
#include <limits.h>
#include "player.h"
#include "map.h"
#include "graphics.h"
#include "utils.h"

ray_t rays[NUM_RAYS];

static bool facingDown(float angle)
{
    return angle > 0 && angle < PI;
}

static bool facingUp(float angle)
{
    return !facingDown(angle);
}

static bool facingRight(float angle)
{
    return angle < PI * 0.5 || angle > PI * 1.5;
}

static bool facingLeft(float angle)
{
    return !facingRight(angle);
}

void castRay(float rayAngle, int rayId)
{
    ray_t* ray = &rays[rayId];
    ray->angle = rayAngle;
    normalizeAngle(&ray->angle);

    // Horizontal intersections
    float yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    if (facingDown(ray->angle))
        yintercept += TILE_SIZE;
    
    float xintercept = player.x + ((yintercept - player.y) / tan(ray->angle));
    float ystep = TILE_SIZE;
    if (facingUp(ray->angle))
        ystep *= -1;

    float xstep = TILE_SIZE / tan(ray->angle);
    if ((facingLeft(ray->angle) && xstep > 0) || (facingRight(ray->angle) && xstep < 0))
        xstep *= -1;
    
    float nextIntersectionX = xintercept;
    float nextIntersectionY = yintercept;
    
    bool horizontalWallHitFound = false;
    float horizontalWallHitX = 0;
    float horizontalWallHitY = 0;
    int horizontalWallTexture = 0;

    while (isInsideMapBounds(nextIntersectionX, nextIntersectionY))
    {
        int gridTexture = getGridTexture(nextIntersectionX, nextIntersectionY - (facingDown(ray->angle) ? 0 : 1));
        if (gridTexture != 0)
        {
            // Hit a wall
            horizontalWallHitFound = true;
            horizontalWallHitX = nextIntersectionX;
            horizontalWallHitY = nextIntersectionY;
            horizontalWallTexture = gridTexture;
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
    if (facingRight(ray->angle))
        xintercept += TILE_SIZE;
    
    yintercept = player.y + ((xintercept - player.x) * tan(ray->angle));
    xstep = TILE_SIZE;
    if (facingLeft(ray->angle))
        xstep *= -1;

    ystep = TILE_SIZE * tan(ray->angle);
    if ((facingUp(ray->angle) && ystep > 0) || (facingDown(ray->angle) && ystep < 0))
        ystep *= -1;
    
    nextIntersectionX = xintercept;
    nextIntersectionY = yintercept;
    
    bool verticalWallHitFound = false;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallTexture = 0;

    while (isInsideMapBounds(nextIntersectionX, nextIntersectionY))
    {
        int gridTexture = getGridTexture(nextIntersectionX - (facingRight(ray->angle) ? 0 : 1), nextIntersectionY);
        if (gridTexture != 0)
        {
            // Hit a wall
            verticalWallHitFound = true;
            verticalWallHitX = nextIntersectionX;
            verticalWallHitY = nextIntersectionY;
            verticalWallTexture = gridTexture;
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
        ray->texture = verticalWallTexture;
        ray->wasHitVertical = true;
    }
    else
    {
        ray->distance = xDist;
        ray->wallHitX = horizontalWallHitX;
        ray->wallHitY = horizontalWallHitY;
        ray->texture = horizontalWallTexture;
        ray->wasHitVertical = false;
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

void renderMapRays()
{
    for (int i = 0; i < NUM_RAYS; i++)
    {
        drawScaledLine(player.x, player.y, rays[i].wallHitX, rays[i].wallHitY, 0xFF0000FF);
    }
}
