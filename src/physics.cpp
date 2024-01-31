#include "include/game.h"
#include <stdio.h>
#include <algorithm>

V2 far_away = {INFINITY,INFINITY};


void CollisionHandler::OnCollide(Collider* collider){}

bool AABB::intersects(AABB& other) const{
    if(position.x == INFINITY || other.position.x == INFINITY || position.y == INFINITY || other.position.y == INFINITY){ return false; }
    bool intersectsX =  position.x < (other.position.x + other.size.x) && 
                        other.position.x < (position.x + size.x);
    bool intersectsY = position.y < (other.position.y + other.size.y) && 
                        other.position.y < (position.y + size.y);
    return intersectsX && intersectsY;
}

AABB AABB::translate(V2& v2) const{
    return {{position.x+v2.x, position.y + v2.y}, size};
}






float clamp(float first, float second){
    if(second == 0){
        return 0;
    }else if(second > 0){
        return std::min(first, second);
    }else{
        return std::max(first, second);
    }
}


V2 AABB:: get_collided_movement_vector(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    AABB oldAABB = {position, size};
    this->position = far_away;
    V2 res = v2;
    for(int i=0;i < level->collider_count;i++){
        if(newAABB.intersects(level->collider[i].aabb)){
            V2 move_into = oldAABB.get_amount_can_move_into(&level->collider[i], v2, level);
            res.x = clamp(res.x, move_into.x);
            res.y = clamp(res.y, move_into.y);
        }
    }
    this->position = oldAABB.position;
    return res;
}


V2 AABB::move_and_collide(V2 v2, Level* level){
    V2 actual_movement = get_collided_movement_vector(v2, level);
    if(actual_movement.x != 0 || actual_movement.y != 0){
        move_and_push_boxes(actual_movement,level);
    }
    return actual_movement;
}

void AABB::move_and_push_boxes(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    position = far_away;
        for(int i=0;i < level->collider_count;i++){
            if (newAABB.intersects(level -> collider[i].aabb)){
                collision_response(&level -> collider[i], v2, level);
            }
        }
    position = newAABB.position;
}

void AABB::collision_response(Collider* collider, V2 v2, Level* level){
    switch(collider->collision_type){
        case 0: return;
        case 1: collider->aabb.move_and_collide(v2, level); return;
        case 2:
            if(collider->collision_handler != NULL){
                collider->collision_handler->OnCollide(collider);
                return;
            } 
    }
}







V2 AABB::get_distance_vector(AABB& other, bool x_positive, bool y_positive) const{
    V2 res;
            if(x_positive){
                res.x = other.position.x - position.x - size.x;
            }else{
                res.x = other.position.x + other.size.x - position.x;
            }
            if(y_positive){
                res.y = other.position.y - position.y - size.y;
            }else{
                res.y = other.position.y + other.size.y - position.y;
            }
            return res;
}

V2 AABB::get_amount_can_move_into(Collider* collider, V2 v2, Level* level){
    switch(collider->collision_type){
        case 0:
            return get_distance_vector(collider->aabb, v2.x >=0, v2.y >= 0);
        case 1:
            return collider->aabb.get_collided_movement_vector(v2,level) + 
                get_distance_vector(collider->aabb, v2.x >=0, v2.y >= 0);
        case 2: return v2;
    }
}








