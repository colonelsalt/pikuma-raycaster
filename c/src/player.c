#include "player.h"

#include "defs.h"
#include "map.h"
#include "graphics.h"
#include "utils.h"

player_t player = {
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
    .width = 1,
    .height = 1,
    .turnDirection = 0,
    .walkDirection = 0,
    .rotationAngle = PI / 2.0f,
    .walkSpeed = 100,
    .turnSpeed = 45 * (PI / 180.0f)
};

void movePlayer(float deltaTime)
{
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
    normalizeAngle(&player.rotationAngle);

    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

    float newX = player.x + cos(player.rotationAngle) * moveStep;
    float newY = player.y + sin(player.rotationAngle) * moveStep;

    if (getGridTexture(newX, newY) == 0)
    {
        player.x = newX;
        player.y = newY;
    }
}

void renderMapPlayer()
{
    drawScaledRect(player.x, player.y, player.width, player.height, 0xFFFFFFFF);
    
    // drawScaledLine(
    //     player.x,
    //     player.y,
    //     player.x + cos(player.rotationAngle) * 40,
    //     player.y + sin(player.rotationAngle) * 40
    // );
}
