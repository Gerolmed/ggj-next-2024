#ifndef NODE_H
#define NODE_H

#include "include/types.h"
#include "include/math.h"

class Node {

public:
    virtual ~Node() = default;
    Node();

    Node* parent = nullptr;
	V2 position = V2();
    float rotation = 0;



    Mat3f getRelativeMatrix() const;
    Mat3f getAbsoluteMatrix() const;

    virtual void update();
    virtual void render();
};

#endif //NODE_H
