#include "include/game.h"

#include <assert.h>

void game_Init(Level* level, u32 stage, Arena* arena)
{
    level->static_collider = 0;
    level->collider_count = 0;

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

            if (type) {
                Collider collider;
                collider.aabb.position.x = x * TILE_SIZE;
                collider.aabb.position.y = y * TILE_SIZE;
                collider.aabb.size.x = TILE_SIZE;
                collider.aabb.size.y = TILE_SIZE;
                game_PushCollider(level, collider, true);
            }
        }
    }
    level->grid[4 + level->grid_width * 3] = 1;
    level->grid[5 + level->grid_width * 3] = 1;
    level->grid[4 + level->grid_width * 4] = 1;
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

void game_PushCollider(Level* level, Collider c, bool static_c)
{
    assert(level->collider_count < MAX_COLLIDERS);
    level->collider[level->collider_count] = c;
    level->collider_count += 1;

    if (static_c) {
        level->static_collider += 1;
    }
}

void game_ClearDynamicCollider(Level* level)
{
    level->collider_count = level->static_collider;
}
