#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

bool initWindow(void);
void destroyWindow(void);
void clearColorBuffer(uint32_t color);
void renderColorBuffer(void);
void setPixel(int x, int y, uint32_t color);
void drawScaledLine(int x0, int y0, int x1, int y1, uint32_t color);
void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
void drawRect(int x, int y, int width, int height, uint32_t color);
void drawScaledRect(float x, float y, float width, float height, uint32_t color);

#endif