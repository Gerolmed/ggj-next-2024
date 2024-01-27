#include "include/physics.h"


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