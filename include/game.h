#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"
#include "include/arena.h"

#define MAX_COLLIDERS 150
#define TILE_SIZE 60


struct Level;

enum TileType
{
    Floor = 0,
    Wall = 1,
};

struct Camera
{
    V2 center;
};

struct AABB {
    V2 position;
    V2 size;

    bool intersects(AABB& other) const;
    AABB translate(V2& v2) const;
    //Tries to move. Fails if it hits a wall. Pushes all boxes in the way.
    bool move_and_collide(V2& v2, Level* level);
    //Checks whether it can move.
    bool can_move(V2& v2, Level* level);
    //Moves and pushes boxes, ignoring wall collisions.
    void move_and_push_boxes(V2& v2, Level* level);
};


struct Collider
{
    AABB aabb;
    bool movable;
};


struct Level
{
    u8* grid;
    i32 grid_width;
    i32 grid_height;

    Collider collider[MAX_COLLIDERS];
    u32 collider_count;
    u32 static_collider;

    Camera camera;
};

void game_Init(Level* level, u32 stage, Arena* arena);
void game_RenderGrid(CommandBuffer* cmd, 
                     Level* level, 
                     TextureHandle* texture);
void game_PushCollider(Level* level, Collider c, bool static_c);
void game_ClearDynamicCollider(Level* level);

float game_Raycast(Level* level, V2 pos, V2 dir);

#endif
