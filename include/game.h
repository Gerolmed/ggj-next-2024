#ifndef GAME_H
#define GAME_H

#include "include/types.h"
#include "include/renderer.h"

#define MAX_COLLIDERS 400
#define TILE_SIZE 32
#define HALF_TILE_SIZE 16


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


struct Collider;

struct AABB {
    V2 position;
    V2 size;

    bool intersects(AABB& other) const;
    AABB translate(V2& v2) const;
    //Tries to move. Stops at walls. Returns how far it actually moved. Pushes all boxes in the way.
    V2 move_and_collide(V2 v2, Level* level);
    //Returns how far one can move along v2 until one hits an immovable wall.
    V2 get_collided_movement_vector(V2& v2, Level* level);
    //Moves and pushes boxes, ignoring wall collisions.
    void move_and_push_boxes(V2& v2, Level* level);

    V2 get_amount_can_move_into( Collider* collider, V2 v2, Level* level);

    void collision_response( Collider* collider, V2 v2, Level* level);

    V2 get_distance_vector(AABB& other, bool x_positive, bool y_positive) const;
};

//Helper function in collision system.
//If first and second have the same sign, it's the number of smaller magnitude.
//If they have different sign it's just first.
float clamp(float first, float second);



class CollisionHandler {
public:
    virtual void OnCollide(Collider* collider);
};



struct Collider
{
    AABB aabb;
    i32 collision_type;
    //0: Wall
    //1: Box
    //2: Noclip

    i32 transparency_type;
    //0: Opaque
    //1: Transparent
    //2: Mirror [not yet implemented]

    CollisionHandler* collision_handler;
};


struct StageAttributes {
    u32 max_score;
    u32 score_requirement;
    u32 time_limit;
};
struct Level
{
    u8* grid;
    i32 grid_width;
    i32 grid_height;
    StageAttributes* stage_attributes;
    u32 current_score;

    bool resetStage;
    bool ffGoNext;

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
