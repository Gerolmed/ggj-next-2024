#ifndef PHYSICS_H
#define PHYSICS_H
#include "include/types.h"
#include "include/game.h"



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
    bool move_and_push_boxes(V2& v2, Level* level);

};


#endif //PYSHICS_H