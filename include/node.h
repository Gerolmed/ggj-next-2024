#ifndef NODE_H
#define NODE_H

#include "include/types.h"
#include "include/math.h"

class Node {
    Node* parent;
	V2 position;
    float rotation,

    Matrix3f getRelativeMatrix();
    Matrix3f getAbsoluteMatrix();
}

#endif //NODE_H
