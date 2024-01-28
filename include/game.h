#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"

#define MAX_COLLIDERS 150
#define TILE_SIZE 32


struct Level;

enum TileType
{
    Floor = 0,
    Wall = 1,
    Box = 2,
    Player = 3,
    Enemy = 4,
    Objective = 5
};

struct Camera
{
    V2 center;
};

struct AABB;

class CollisionHandler {
public:
    virtual void OnCollide();
};

struct Collider
{
    AABB aabb;
    i32 collision_type;
    //0: Wall
    //1: Box
    //2: Collectable

    CollisionHandler* collision_handler;
};



struct AABB {
    V2 position;
    V2 size;

    bool intersects(AABB& other) const;
    AABB translate(V2& v2) const;
    //Tries to move. Fails if it hits a wall. Pushes all boxes in the way.
    bool move_and_collide(V2 v2, Level* level);
    //Checks whether it can move.
    bool can_move(V2& v2, Level* level);
    //Moves and pushes boxes, ignoring wall collisions.
    void move_and_push_boxes(V2& v2, Level* level);

    bool can_move_into(Collider collider, V2 v2, Level* level);

    void collision_response(Collider collider, V2 v2, Level* level);
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


void game_RenderGrid(CommandBuffer* cmd,
                     Level* level, 
                     TextureHandle wall_texture,
                     TextureHandle floor_texture);

void game_RenderBoxes(CommandBuffer* cmd,
                     Level* level, 
                     TextureHandle texture);
void game_PushCollider(Level* level, Collider c, bool static_c);
void game_ClearDynamicCollider(Level* level);

float game_Raycast(Level* level, V2 pos, V2 dir);

#endif
