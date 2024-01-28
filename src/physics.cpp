#include "include/game.h"
#include <stdio.h>

V2 far_away = {INFINITY,INFINITY};


void CollisionHandler::OnCollide(){}

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



bool AABB:: can_move(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    V2 old_position = position;
    this->position = far_away;
    for(int i=0;i < level->collider_count;i++){
        if(newAABB.intersects(level->collider[i].aabb)){
            if(!can_move_into(&level->collider[i], v2, level)){
                position = old_position;
                return false;
            }
        }
    }
    position = old_position;
    return true;
}


bool AABB::move_and_collide(V2 v2, Level* level){
    if(can_move(v2, level)){
        move_and_push_boxes(v2,level);
        return true;
    }
    return false;
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

void collision_response(Collider* collider, V2 v2, Level* level){
    switch(collider->collision_type){
        case 0: return;
        case 1: collider->aabb.move_and_push_boxes(v2, level); return;
        case 2:
            if(collider->collision_handler != NULL){
                collider->collision_handler->OnCollide();
                return;
            } 
    }
}


bool can_move_into(Collider* collider, V2 v2, Level* level){

    switch(collider->collision_type){
        case 0: return false;
        case 1: return collider->aabb.can_move(v2,level);
        case 2: return true;
    }
}








