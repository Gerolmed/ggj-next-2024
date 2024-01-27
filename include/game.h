#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"

struct Camera
{
    float center_x;
    float center_y;
};

struct Level
{
    u8* grid;
    u32 grid_width;
    u32 grid_height;
};


void game_Init(Level* level);
void game_RenderGrid(CommandBuffer* cmd, 
                     Level* level, 
                     TextureHandle* texture);

#endif
