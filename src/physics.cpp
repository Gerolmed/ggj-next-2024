#include "include/physics.h"


V2 far_away = {INFINITY,INFINITY};

bool AABB::intersects(AABB& other) const{
    bool intersectsX =  position.x <= (other.position.x + other.size.x) && 
                        other.position.x <= (position.x + size.x);
    bool intersectsY = position.y <= (other.position.y + other.size.y) && 
                        other.position.y <= (position.y + size.y);
    return intersectsX && intersectsY;
}

AABB AABB::translate(V2& v2) const{
    return {{position.x+v2.x, position.y + v2.y}, size};
}

bool AABB::move_and_collide(V2& v2, Level* level){
    AABB newAABB = translate(v2);
    V2 old_position = position;
    position = far_away;
    for(int i=0;i < level->collider_count;i++){
        if(newAABB.intersects(level->collider[i].aabb)){
            position = old_position;
            return false;
        }
    }
    position = newAABB.position;
    return true;
}







