#include "include/physics.h"


V2 far_away = {INFINITY,INFINITY};

bool AABB::intersects(AABB& other) const{
    if(position.x == INFINITY){ return false; }
    bool intersectsX =  position.x <= (other.position.x + other.size.x) && 
                        other.position.x <= (position.x + size.x);
    bool intersectsY = position.y <= (other.position.y + other.size.y) && 
                        other.position.y <= (position.y + size.y);
    return intersectsX && intersectsY;
}

AABB AABB::translate(V2& v2) const{
    return {{position.x+v2.x, position.y + v2.y}, size};
}

bool AABB:: can_move(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    V2 old_position = position;
    position = far_away;
    for(int i=0;i < level->collider_count;i++){
        if(newAABB.intersects(level->collider[i].aabb)){
            if(!level->collider[i].movable  || !level->collider[i].aabb.can_move(v2,level)){
                position = old_position;
                return false;
            }
        }
    }
    position = old_position;
    return true;
}


bool AABB::move_and_collide(V2& v2, Level* level){
    if(can_move(v2, level)){
        move_and_push_boxes(v2,level);
    }
}

bool AABB::move_and_push_boxes(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    position = far_away;
        for(int i=0;i < level->collider_count;i++){
            if(level->collider[i].movable && newAABB.intersects(level->collider[i].aabb)){
                level->collider[i].aabb.move_and_push_boxes(v2,level);
            }
        }
    position = newAABB.position;
}







