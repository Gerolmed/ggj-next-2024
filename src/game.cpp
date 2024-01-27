#include "include/game.h"
#include "include/game_init.h"
#include "include/stb_image.h"

#include <assert.h>

#include "include/arena.h"
#include "include/game_nodes.h"

void game_Init(Level* level, u32 stage, Arena* arena, Node* scene_root)
{
    level->static_collider = 0;
    level->collider_count = 0;
    level->camera.center = v2(10.5 * TILE_SIZE, 5.5 * TILE_SIZE);

    char path[1024];
    sprintf(path,"assets/stages/%d.png",stage);
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
            u32 index = 3*(x+(y*level->grid_width));
            u8 type = Floor;


            if (temp[index] == 255 &&
                temp[index+1] == 255 &&
                temp[index+2] == 255)
                type = Floor;
            if (temp[index] == 0 &&
                temp[index+1] == 0 &&
                temp[index+2] == 0)
                type = Wall;
            if (temp[index] == 88 &&
                temp[index+1] == 57 &&
                temp[index+2] == 39)
                type = Box;
            if (temp[index] == 255 &&
                temp[index+1] == 0 &&
                temp[index+2] == 0)
                type = Player;



            level->grid[index/3] = type;

            if (type == Box || type == Wall) {
                Collider collider;
                collider.aabb.position.x = x * TILE_SIZE;
                collider.aabb.position.y = y * TILE_SIZE;
                collider.aabb.size.x = TILE_SIZE;
                collider.aabb.size.y = TILE_SIZE;
                game_PushCollider(level, collider, true);
            }
            else if(type == Player){

                const auto player_node = new PlayerNode(level);
                player_node->position = v2(x * TILE_SIZE, y * TILE_SIZE);
                scene_root->AddChild(player_node);
                
            }
        }
    }
    stbi_image_free(temp);
}

u64 CalcIndex(int x, int y, int width, int height) {
    if(x >= width) {
        x = width-1;
    } else if(x < 0) {
        x = 0;
    }
    if(y >= height) {
        y = height-1;
    } else if(y < 0) {
        y = 0;
    }

    return (x)+(y)*width;
}

u8 GetSpriteIdFromGridPos(Level* level,u32 x, u32 y, u8 type) {
    bool one = level->grid[CalcIndex(x-1,y+1,level->grid_width,level->grid_height)]==type;
    bool two = level->grid[CalcIndex(x,y+1,level->grid_width,level->grid_height)]==type;
    bool three = level->grid[CalcIndex(x+1,y+1,level->grid_width,level->grid_height)]==type;

    bool four = level->grid[CalcIndex(x-1,y,level->grid_width,level->grid_height)]==type;
    bool six = level->grid[CalcIndex(x+1,y,level->grid_width,level->grid_height)]==type;

    bool seven = level->grid[CalcIndex(x-1,y-1,level->grid_width,level->grid_height)]==type;
    bool eight = level->grid[CalcIndex(x,y-1,level->grid_width,level->grid_height)]==type;
    bool nine = level->grid[CalcIndex(x+1,y-1,level->grid_width,level->grid_height)]==type;
    u8 mask = one << 7 | two << 6 | three << 5 | four << 4 | six << 3 | seven << 2 | eight << 1 | nine;

    switch (mask) {
        case 0b00000010: return 0;
        case 0b00001010: return 1;
        case 0b00011010: return 2;
        case 0b00010010: return 3;
        case 0b11011010: return 4;
        case 0b00111011: return 5;
        case 0b10011110: return 6;
        case 0b01111010: return 7;
        case 0b00001011: return 8;
        case 0b01011111: return 9;
        case 0b00011111: return 10;
        case 0b00010110: return 11;

        case 0b01000010: return 12;
        case 0b01001010: return 13;
        case 0b01011010: return 14;
        case 0b01010010: return 15;
        case 0b01001011: return 16;
        case 0b01111111: return 17;
        case 0b11011111: return 18;
        case 0b01010110: return 19;
        case 0b01101011: return 20;
        case 0b01111110: return 21;
        case 0b11011110: return 23;

        case 0b01000000: return 24;
        case 0b01001000: return 25;
        case 0b01011000: return 26;
        case 0b01010000: return 27;
        case 0b01101010: return 28;
        case 0b11111011: return 29;
        case 0b11111110: return 30;
        case 0b11010010: return 31;
        case 0b01111011: return 32;
        case 0b11111111: return 33;
        case 0b11011011: return 34;
        case 0b11010110: return 35;

        case 0b00000000: return 36;
        case 0b00001000: return 37;
        case 0b00011000: return 38;
        case 0b00010000: return 39;
        case 0b01011110: return 40;
        case 0b01111000: return 41;
        case 0b11011000: return 42;
        case 0b01011011: return 43;
        case 0b01101000: return 44;
        case 0b11111000: return 45;
        case 0b11111010: return 46;
        case 0b11010000: return 47;

        default: return 33;
    }






}

void game_RenderGrid(CommandBuffer* cmd, Level* level, TextureHandle texture,TextureHandle wall_texture)
{
    renderer_PushBase(cmd, level->camera.center);
    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u8 type = level->grid[x + level->grid_width * y];

            V3 color = {0, 0, 0};

            if (type == Wall) {

                const u8 current_frame = GetSpriteIdFromGridPos(level, x, y, type);
                printf("x:%d y:%d id:%d\n", x, y,current_frame);
                const float item_height = 1.0f / 4;
                const float item_width = 1.0f / 12;
                const float remainingX = current_frame % 12;
                const V2 uv_bot_left = V2{remainingX * item_width, ((current_frame - remainingX) / 12 + 1) * item_height};
                const V2 uv_top_right = uv_bot_left + V2{item_width, item_height};


                renderer_PushSprite(cmd, v2(x * TILE_SIZE, y * TILE_SIZE),
                                    v2((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE),
                                    0,
                                    uv_bot_left, uv_top_right,
                                    mat2(1), v3(1), wall_texture);
                continue;
            }
            if (type == Box) {
                color = v3(0.3, 0.225, 0.15);
            }
            renderer_PushSprite(cmd, v2(x * TILE_SIZE, y * TILE_SIZE), 
                                v2((x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE),
                                0,
                                v2(0), v2(1),
                                mat2(1), color, texture);
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

float game_Raycast(Level* level, V2 pos, V2 dir)
{
    float min_t = 99999999;
    for (u32 i = 0; i < level->collider_count; ++i) {
        Collider* col = level->collider + i;

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

            }

            {
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

            }

            {
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

