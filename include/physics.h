#ifndef PHYSICS_H
#define PHYSICS_H
#include "include/types.h"


struct AABB {
    V2 position;
    V2 size;

    bool intersects(AABB& other) const;
    
    AABB translate(V2& v2) const;

};


#endif //PYSHICS_H