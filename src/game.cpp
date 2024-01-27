#include "include/game.h"

#define TILE_SIZE 30

void game_Init(Level* level, u32 stage, Arena* arena)
{
    level->grid_width = 10;
    level->grid_height = 10;
    level->grid = (u8*) push_size(arena, 
                sizeof(u8) * level->grid_width * level->grid_height);
    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u8 type = 0;
            if (x == 0 || y == 0 || x == 9 || y == 9) {
                type = 1;
            }
            level->grid[x + y * level->grid_width] = type;
        }
    }
}

void game_RenderGrid(CommandBuffer* cmd, Level* level, TextureHandle* texture)
{
    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u8 type = level->grid[x + level->grid_width * y];

            if (type == Wall) {
                renderer_PushSprite(cmd, 
                    v2(x * TILE_SIZE, y * TILE_SIZE), 
                    v2((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE), 
                    v2(0), v2(1),
                    mat2(1), v3(0, 1, 0), texture);
            }
        }
    }
}
