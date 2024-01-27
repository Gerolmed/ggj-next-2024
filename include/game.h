#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"
#include "include/arena.h"

#define MAX_COLLIDERS 150


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

struct Collider
{
    AABB aabb;
};

struct Level
{
    u8* grid;
    u32 grid_width;
    u32 grid_height;

    Collider collider[MAX_COLLIDERS];
    u32 collider_count;
    u32 static_collider;
};

void game_Init(Level* level, u32 stage, Arena* arena);
void game_RenderGrid(CommandBuffer* cmd, 
                     Level* level, 
                     TextureHandle* texture);

#endif
