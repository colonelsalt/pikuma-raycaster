#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <math.h>

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64

#define NUM_TEXTURES 14

#define MINIMAP_SCALE_FACTOR 0.1

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FOV_ANGLE (60 * (PI / 180.0))

#define NUM_RAYS WINDOW_WIDTH

#define DIST_PROJ_PLANE ((WINDOW_WIDTH / 2.0) / tan(FOV_ANGLE / 2.0))

#define FPS 30
#define TARGET_FRAME_TIME (1000 / FPS)

typedef uint32_t color_t;

#endif