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
    int texture;
} ray_t;

extern ray_t rays[NUM_RAYS];

void normalizeAngle(float* angle);
float distance(float px, float py, float qx, float qy);
void castRay(float rayAngle, int rayId);
void castAllRays();
void renderMapRays();

#endif