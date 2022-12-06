#include "texture.h"

texture_t wallTextures[NUM_TEXTURES];

static const char* textureFilenames[NUM_TEXTURES] = {
    "./textures/redbrick.png",
    "./textures/purplestone.png",
    "./textures/mossystone.png",
    "./textures/graystone.png",
    "./textures/colorstone.png",
    "./textures/bluestone.png",
    "./textures/wood.png",
    "./textures/eagle.png",
    "./textures/pikuma.png"
};

void loadWallTextures()
{
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        upng_t* upng = upng_new_from_file(textureFilenames[i]);
        if (upng)
        {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK)
            {
                wallTextures[i].upngTexture = upng;
                wallTextures[i].width = upng_get_width(upng);
                wallTextures[i].height = upng_get_height(upng);
                wallTextures[i].textureBuffer = (uint32_t*) upng_get_buffer(upng);
            }
        }
    }
}

void freeWallTextures()
{
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        upng_free(wallTextures[i].upngTexture);
    }
}