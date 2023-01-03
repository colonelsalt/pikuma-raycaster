#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

int getGridTexture(float x, float y);

bool isInsideMapBounds(float x, float y);

void renderMapGrid();

#endif