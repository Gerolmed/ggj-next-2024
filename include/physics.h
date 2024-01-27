#ifndef PHYSICS_H
#define PHYSICS_H
#include "include/types.h"




struct AABB {
    V2 position;
    V2 size;

    bool intersects(AABB& other) const;
    
    AABB translate(V2& v2) const;

    bool move_and_collide(V2& v2);

};


#endif //PYSHICS_H