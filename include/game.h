#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"
#include "include/arena.h"


enum TileType
{
    Floor = 0,
    Wall = 1,
};

struct Camera
{
    float center_x;
    float center_y;
};

struct Level
{
    u8* grid;
    i32 grid_width;
    i32 grid_height;
};


void game_Init(Level* level, u32 stage, Arena* arena);
void game_RenderGrid(CommandBuffer* cmd, 
                     Level* level, 
                     TextureHandle* texture);

#endif
