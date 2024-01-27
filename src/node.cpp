#include "include/node.h"

Mat3f Node::getAbsoluteMatrix() const {
    if (parent == nullptr) return getRelativeMatrix();
    return parent->getAbsoluteMatrix() * getRelativeMatrix();
}

Mat3f Node::getRelativeMatrix() const {
    float s, c;
    sincosf(rotation, &s, &c);
    return Mat3f{{
               c, -s, position.x,
               s, c, position.y,
               0.0f, 0.0f, 1.0f
           }};
}
