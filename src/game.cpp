#include "include/game.h"
#include "include/game_init.h"
#include "include/stb_image.h"
#include <assert.h>
#include "include/arena.h"
#include "include/map.h"

void game_Init(Level* level, u32 stage, Arena* arena, Node* scene_root, StageAttributes* attributes) {
    level->static_collider = 0;
    level->collider_count = 0;
    level->current_score = 0;
    level->camera.center = v2(10.5 * TILE_SIZE, 10.5 * TILE_SIZE);
    map_init(level, stage, arena, scene_root,attributes);
}

void game_RenderBoxes(CommandBuffer* cmd, Level* level, TextureHandle texture) {
    renderer_PushBase(cmd, level->camera.center);

    V3 color = {0, 0, 0};
    Collider collider;
    for (int i = 0; i < level->collider_count; i++) {
        collider = level->collider[i];
        if (collider.collision_type == 1) {
            color = v3(0.3, 0.225, 0.15);
            renderer_PushSprite(cmd, v2(collider.aabb.position.x, collider.aabb.position.y),
                                v2(collider.aabb.position.x + collider.aabb.size.x,
                                   collider.aabb.position.y + collider.aabb.size.y),
                                1,
                                sprite(
                                    v2(0),
                                    v2(1)
                                ),
                                mat2(1), color, texture);
        }
    }
}


void game_RenderGrid(CommandBuffer* cmd, Level* level, TextureHandle wall_texture, TextureHandle floor_texture) {
    renderer_PushBase(cmd, level->camera.center);

    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u8 type = level->grid[x + level->grid_width * y];


            if (type == Wall) {
                Sprite sprite = GetSpriteFromGridPos(level, y, x, type);


                renderer_PushSprite(cmd, v2(x * TILE_SIZE, y * TILE_SIZE),
                                    v2((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE),
                                    0,
                                    sprite,
                                    mat2(1), v3(1), wall_texture);
                continue;
            }


            renderer_PushSprite(
                cmd,
                v2(x * TILE_SIZE, y * TILE_SIZE),
                v2((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE),
                0,
                sprite(
                    v2(0),
                    v2(1)
                ),
                mat2(1), v3(1), floor_texture);
        }
    }
}

void game_PushCollider(Level* level, Collider c, bool static_c) {
    assert(level->collider_count < MAX_COLLIDERS);
    level->collider[level->collider_count] = c;
    level->collider_count += 1;

    if (static_c) {
        level->static_collider += 1;
    }
}

void game_ClearDynamicCollider(Level* level) {
    level->collider_count = level->static_collider;
}

float game_Raycast(Level* level, V2 pos, V2 dir) {
    float min_t = 99999999;
    for (u32 i = 0; i < level->collider_count; ++i) {
        Collider* col = level->collider + i;
        if(col->transparency_type == 1){
            continue;
        }

        // x collision
        if (dir.x > 0.00001 || dir.x < 0.00001) {
            {
                float t = (col->aabb.position.x - pos.x) / dir.x;
                float y = t * dir.y + pos.y;
                float y_diff = y - col->aabb.position.y;

                if (y_diff >= 0 && y_diff <= col->aabb.size.y) {
                    if (t >= 0 && t < min_t) {
                        min_t = t;
                    }
                }
            } {
                float t = (col->aabb.position.x + col->aabb.size.x - pos.x) / dir.x;
                float y = t * dir.y + pos.y;
                float y_diff = y - col->aabb.position.y;

                if (y_diff >= 0 && y_diff <= col->aabb.size.y) {
                    if (t >= 0 && t < min_t) {
                        min_t = t;
                    }
                }
            }
        }

        // y collision
        if (dir.y > 0.00001 || dir.y < 0.00001) {
            {
                float t = (col->aabb.position.y - pos.y) / dir.y;
                float x = t * dir.x + pos.x;
                float x_diff = x - col->aabb.position.x;

                if (x_diff >= 0 && x_diff <= col->aabb.size.x) {
                    if (t >= 0 && t < min_t) {
                        min_t = t;
                    }
                }
            } {
                float t = (col->aabb.position.y + col->aabb.size.y - pos.y) / dir.y;
                float x = t * dir.x + pos.x;
                float x_diff = x - col->aabb.position.x;

                if (x_diff >= 0 && x_diff <= col->aabb.size.x) {
                    if (t >= 0 && t < min_t) {
                        min_t = t;
                    }
                }
            }
        }
    }

    return min_t;
}
