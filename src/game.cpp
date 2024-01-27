#include "include/game.h"
#include "include/stb_image.h"

#include <assert.h>

void game_Init(Level* level, u32 stage, Arena* arena)
{
    level->static_collider = 0;
    level->collider_count = 0;
    level->camera.center_x = 5 * TILE_SIZE;
    level->camera.center_y = 5 * TILE_SIZE;

    char path[1024];
    sprintf(path,"assets/stages/%d.png",stage);
    printf("%s\n", path);
    u8* temp = stbi_load(
            path,
            &level->grid_width,
            &level->grid_height,
            NULL,
            STBI_rgb
    );
    assert(temp);
    level->grid = (u8*) push_size(arena, sizeof(u8) * level->grid_width * level->grid_height);

    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {

            u8 type = Floor;
            u32 index = 3*(x+(y*level->grid_width));
            if (temp[index] == 255 && 
                temp[index+1] == 0 && 
                temp[index+2] == 0) 
                type = Wall;
            level->grid[index/3] = type;

            if (type == Wall) {
                Collider collider;
                collider.aabb.position.x = x * TILE_SIZE;
                collider.aabb.position.y = y * TILE_SIZE;
                collider.aabb.size.x = TILE_SIZE;
                collider.aabb.size.y = TILE_SIZE;
                game_PushCollider(level, collider, true);
            }
        }
    }
    stbi_image_free(temp);
}

void game_RenderGrid(CommandBuffer* cmd, Level* level, TextureHandle* texture)
{
    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u8 type = level->grid[x + level->grid_width * y];

            if (type == Wall) {
                renderer_PushSprite(cmd, 
                    v2(x * TILE_SIZE - level->camera.center_x, 
                       y * TILE_SIZE - level->camera.center_y), 
                    v2((x + 1) * TILE_SIZE - level->camera.center_x, 
                       (y + 1) * TILE_SIZE - level->camera.center_y), 
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
