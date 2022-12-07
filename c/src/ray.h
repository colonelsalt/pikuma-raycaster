#ifndef RAY_H
#define RAY_H

#include <stdbool.h>

#include "defs.h"

typedef struct
{
    float angle;
    float wallHitX, wallHitY;
    float distance;
    bool wasHitVertical;
    int wallHitContent;
} ray_t;

extern ray_t rays[NUM_RAYS];

float normalizeAngle(float angle);
float distance(float px, float py, float qx, float qy);
void castRay(float rayAngle, int rayId);
void castAllRays();
void renderRays();

#endif