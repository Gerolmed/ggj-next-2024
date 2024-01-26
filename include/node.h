#ifndef NODE_H
#define NODE_H

#include "include/types.h"
#include "include/math.h"

class Node {

public:
    virtual ~Node() = default;
    Node();

    Node* parent;
	V2 position = V2();
    float rotation = 0;



    Matrix3f getRelativeMatrix();
    Matrix3f getAbsoluteMatrix();

    virtual void update();
    virtual void render();
};

#endif //NODE_H
