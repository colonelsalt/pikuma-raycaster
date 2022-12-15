#include "ray.h"

#include <float.h>
#include <limits.h>
#include "player.h"
#include "map.h"
#include "graphics.h"

ray_t rays[NUM_RAYS];

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
    ray_t* ray = &rays[rayId];
    ray->angle = normalizeAngle(rayAngle);

    bool isRayFacingDown = ray->angle > 0 && ray->angle < PI;
    bool isRayFacingUp = !isRayFacingDown;
    bool isRayFacingRight = ray->angle < PI * 0.5 || ray->angle > PI * 1.5;
    bool isRayFacingLeft = !isRayFacingRight;

    // Horizontal intersections
    float yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    if (isRayFacingDown)
        yintercept += TILE_SIZE;
    
    float xintercept = player.x + ((yintercept - player.y) / tan(ray->angle));
    float ystep = TILE_SIZE;
    if (isRayFacingUp)
        ystep *= -1;

    float xstep = TILE_SIZE / tan(ray->angle);
    if ((isRayFacingLeft && xstep > 0) || (isRayFacingRight && xstep < 0))
        xstep *= -1;
    
    float nextIntersectionX = xintercept;
    float nextIntersectionY = yintercept;
    
    bool horizontalWallHitFound = false;
    float horizontalWallHitX = 0;
    float horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    while (isInsideMapBounds(nextIntersectionX, nextIntersectionY))
    {
        int gridContent = getGridContent(nextIntersectionX, nextIntersectionY - (isRayFacingDown ? 0 : 1));
        if (gridContent != 0)
        {
            // Hit a wall
            horizontalWallHitFound = true;
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
    if (isRayFacingRight)
        xintercept += TILE_SIZE;
    
    yintercept = player.y + ((xintercept - player.x) * tan(ray->angle));
    xstep = TILE_SIZE;
    if (isRayFacingLeft)
        xstep *= -1;

    ystep = TILE_SIZE * tan(ray->angle);
    if ((isRayFacingUp && ystep > 0) || (isRayFacingDown && ystep < 0))
        ystep *= -1;
    
    nextIntersectionX = xintercept;
    nextIntersectionY = yintercept;
    
    bool verticalWallHitFound = false;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallContent = 0;

    while (isInsideMapBounds(nextIntersectionX, nextIntersectionY))
    {
        int gridContent = getGridContent(nextIntersectionX - (isRayFacingRight ? 0 : 1), nextIntersectionY);
        if (gridContent != 0)
        {
            // Hit a wall
            verticalWallHitFound = true;
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
        ray->wasHitVertical = true;
    }
    else
    {
        ray->distance = xDist;
        ray->wallHitX = horizontalWallHitX;
        ray->wallHitY = horizontalWallHitY;
        ray->wallHitContent = horizontalWallContent;
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

void renderRays()
{
    for (int i = 0; i < NUM_RAYS; i++)
    {
        drawScaledLine(player.x, player.y, rays[i].wallHitX, rays[i].wallHitY, 0xFF0000FF);
    }
}
