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
    Node* children[20];
    int child_count = 0;

    Mat3f GetRelativeMatrix() const;
    Mat3f GetAbsoluteMatrix() const;

    virtual void update();
    virtual void render();
};

#endif //NODE_H
