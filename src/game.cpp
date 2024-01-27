#include "include/game.h"

void game_Init(Level* level, u32 stage, Arena* arena)
{
    level->grid_width = 4;
    level->grid_height = 4;
    level->grid = (u8*) push_size(arena, 
                sizeof(u8) * level->grid_width * level->grid_height);
    for (u32 y = 0; y < 4; ++y) {
        for (u32 x = 0; x < 4; ++x) {
            u8 type = 0;
            if (x == 0 || y == 0 || x == 3 || y == 3) {
                type = 1;
            }
            level->grid[x + y * 4] = type;
        }
    }
}

void game_RenderGrid(CommandBuffer* cmd, Level* level, TextureHandle* texture)
{
    for (u32 y = 0; y < 4; ++y) {
        for (u32 x = 0; x < 4; ++x) {
            u8 type = level->grid[x + level->grid_width * y];

            renderer_PushSprite(cmd, 
                                v2(-30), v2(30), 
                                v2(0), v2(1),
                                mat2(1), v3(0, 1, 0), texture);
        }
    }
}
