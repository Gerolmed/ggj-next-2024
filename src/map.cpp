#include <assert.h>
#include "include/map.h"
#include "include/stb_image.h"
#include "include/game_nodes.h"



u8 GetTileTypeFromRgb(const u8* temp, u32 index) {
    u8 type = Floor;
    index *= 3;

    if (temp[index] == 255 &&
        temp[index+1] == 255 &&
        temp[index+2] == 255)
        type = Floor;
    else if (temp[index] == 0 &&
             temp[index+1] == 0 &&
             temp[index+2] == 0)
        type = Wall;
    else if (temp[index] == 88 &&
             temp[index+1] == 57 &&
             temp[index+2] == 39)
        type = Box;
    else if (temp[index] == 255 &&
             temp[index+1] == 0 &&
             temp[index+2] == 0)
        type = Player;
    else if (temp[index] == 0 &&
             temp[index + 1] == 255 &&
             temp[index + 2] == 0)
        type = Enemy;
    else if (temp[index] == 1 &&
             temp[index + 1] == 125)
        type = Objective;

    return type;
}

void map_init(Level* level, u32 stage, Arena* arena, Node* scene_root, StageAttributes* attributes) {
    char path[1024];
    sprintf(path, "assets/stages/%d.png",stage);
    u8* temp = stbi_load(
            path,
            &level->grid_width,
            &level->grid_height,
            NULL,
            STBI_rgb
    );
    assert(temp);
    level->grid_height--;
    level->grid = (u8*) push_size(arena, sizeof(u8) * level->grid_width * (level->grid_height));
    level->stage_attributes = attributes;

    level->stage_attributes->max_score = (u32) temp[0] << 8 | temp[1];
    level->stage_attributes->score_requirement = (u32) temp[3] << 8 | temp[4];
    level->stage_attributes->time_limit = (u32) temp[6] << 8 | temp[7];

    for (u32 y = 0; y < level->grid_height; ++y) {
        for (u32 x = 0; x < level->grid_width; ++x) {
            u32 index = x+(y*level->grid_width);
            u8 type = GetTileTypeFromRgb(temp, index+level->grid_width);
            level->grid[index] = type;

            if (type == Box || type == Wall) {
                Collider collider;
                collider.aabb.position.x = x * TILE_SIZE;
                collider.aabb.position.y = y * TILE_SIZE;
                collider.aabb.size.x = TILE_SIZE;
                collider.aabb.size.y = TILE_SIZE;
                if(type == Box){
                    collider.collision_type = 1;
                }else{
                    collider.collision_type = 0;
                }
                collider.transparency_type = 0;
                game_PushCollider(level, collider, true);
            }
            else if(type == Player){
                PlayerNode* player_node = new PlayerNode(level);
                player_node->position = v2(x * TILE_SIZE + HALF_TILE_SIZE, y * TILE_SIZE + HALF_TILE_SIZE);
                scene_root->AddChild(player_node);

                global_player_pointer = player_node;

            } else if(type == Enemy){

                const auto rotator = new RotatingNode(level);
                rotator->position = v2(x * TILE_SIZE +HALF_TILE_SIZE, y * TILE_SIZE +HALF_TILE_SIZE);
                rotator->speed = -1;
                scene_root->AddChild(rotator);

                const auto enemy_node = new EnemyNode(level);
                rotator->AddChild(enemy_node);
            }
            else if(type == Objective){
                u8 score_value = temp[3*(index+level->grid_width)+2];

                const auto objective_node = new ObjectiveNode(level, score_value);
                objective_node->position =  v2(x * TILE_SIZE + HALF_TILE_SIZE, y * TILE_SIZE + HALF_TILE_SIZE);
                scene_root->AddChild(objective_node);

                Collider collider;
                collider.aabb.position.x = x * TILE_SIZE;
                collider.aabb.position.y = y * TILE_SIZE;
                collider.aabb.size.x = TILE_SIZE;
                collider.aabb.size.y = TILE_SIZE;

                collider.collision_handler = objective_node;
                collider.collision_type = 2;

                collider.transparency_type = 0;

                game_PushCollider(level, collider, true);

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

u8 getAdjacentTileBitmask(const Level *level, u32 x, u32 y, u8 type) {
    bool one = level->grid[CalcIndex(x - 1, y + 1, level->grid_width, level->grid_height)] == type;
    bool two = level->grid[CalcIndex(x,y+1,level->grid_width,level->grid_height)]==type;
    bool three = level->grid[CalcIndex(x+1,y+1,level->grid_width,level->grid_height)]==type;

    bool four = level->grid[CalcIndex(x-1,y,level->grid_width,level->grid_height)]==type;
    bool six = level->grid[CalcIndex(x+1,y,level->grid_width,level->grid_height)]==type;

    bool seven = level->grid[CalcIndex(x-1,y-1,level->grid_width,level->grid_height)]==type;
    bool eight = level->grid[CalcIndex(x,y-1,level->grid_width,level->grid_height)]==type;
    bool nine = level->grid[CalcIndex(x+1,y-1,level->grid_width,level->grid_height)]==type;
    u8 mask = one << 7 | two << 6 | three << 5 | four << 4 | six << 3 | seven << 2 | eight << 1 | nine;
    return mask;
}

u8 GetSpriteIdFromGridPos(Level* level, u32 x, u32 y, u8 type) {
    u8 mask = getAdjacentTileBitmask(level, x, y, type);

    switch (mask) {
        case 0b00000010: return 0;
        case 0b00001010: return 1;
        case 0b00011010: return 2;
        case 0b00010010: return 3;
        case 0b11011010: return 4;
        case 0b00011011:
        case 0b00111011: return 5;
        case 0b00011110:
        case 0b10011110: return 6;
        case 0b01111010: return 7;
        case 0b00001111:
        case 0b00101111:
        case 0b00101011:
        case 0b00001011: return 8;
        case 0b01011111: return 9;
        case 0b10011111:
        case 0b00111111:
        case 0b10111111:
        case 0b00011111: return 10;
        case 0b00010111:
        case 0b10010110:
        case 0b10010111:
        case 0b00010110: return 11;

        case 0b11000010:
        case 0b01100010:
        case 0b11100010:
        case 0b01000111:
        case 0b01000110:
        case 0b01000011:
        case 0b11000110:
        case 0b01000010: return 12;
        case 0b01001010: return 13;
        case 0b01011010: return 14;
        case 0b01010010: return 15;
        case 0b01001011: return 16;
        case 0b01111111: return 17;
        case 0b11011111: return 18;
        case 0b01010110: return 19;
        case 0b11101011:
        case 0b01101111:
        case 0b11101111:
        case 0b01101011: return 20;
        case 0b01111110: return 21;
        case 0b11011110: return 23;

        case 0b01100000:
        case 0b11000000:
        case 0b11100000:
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
        case 0b11110110:
        case 0b11010111:
        case 0b11010110: return 35;

        case 0b00000000: return 36;
        case 0b00001000: return 37;
        case 0b00111001:
        case 0b00111000:
        case 0b00011100:
        case 0b00011001:
        case 0b10011100:
        case 0b10011000:
        case 0b00011101:
        case 0b00011000: return 38;
        case 0b00010000:
        case 0b10010100:
        case 0b00010100:
        case 0b10010000: return 39;
        case 0b01011110: return 40;
        case 0b01111000: return 41;
        case 0b11011000: return 42;
        case 0b01011011: return 43;
        case 0b11101000:
        case 0b01101001:
        case 0b11101001:
        case 0b01101000: return 44;
        case 0b11111100:
        case 0b11111001:
        case 0b11111101:
        case 0b11111000: return 45;
        case 0b11111010: return 46;
        case 0b11010100:
        case 0b11110100:
        case 0b11110000:
        case 0b11010000: return 47;

        default: return 33;
    }
}

Sprite GetSpriteFromGridPos(Level *level, u32 y, u32 x, u8 type) {
    const u8 current_frame = GetSpriteIdFromGridPos(level, x, y, type);
    const int rows = 4;
    const int columns = 12;
    const float item_height = 1.0f / rows;
    const float item_width = 1.0f / columns;
    const float remainingX = current_frame % columns;
    const V2 uv_bot_left = V2{remainingX * item_width, (rows - ((current_frame - remainingX) / columns + 1)) * item_height};
    const V2 uv_top_right= uv_bot_left + V2{item_width, item_height};
    return sprite(uv_bot_left, uv_top_right);
}
