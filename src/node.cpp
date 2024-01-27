#include "include/node.h"

Mat3f Node::GetAbsoluteMatrix() const {
    if (parent == nullptr) return GetRelativeMatrix();
    return parent->GetAbsoluteMatrix() * GetRelativeMatrix();
}

Mat3f Node::GetRelativeMatrix() const {
    float s = sinf(rotation);
    float c = cosf(rotation);
    return Mat3f{{
               c, -s, position.x,
               s, c, position.y,
               0.0f, 0.0f, 1.0f
           }};
}
