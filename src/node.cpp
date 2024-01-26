#include "include/node.h"

Matrix3f Node::getAbsoluteMatrix() const {
    if (parent == nullptr) return getRelativeMatrix();
    return parent->getAbsoluteMatrix() * getRelativeMatrix();
}

Matrix3f Node::getRelativeMatrix() const {
    return Matrix3f{{
               1.0f, 0.0f, position.x,
               0.0f, 1.0f, position.y,
               0.0f, 0.0f, 1.0f
           }} *
            Matrix3f({
               cosf(rotation), -sinf(rotation), 0.0f,
               sinf(rotation), cosf(rotation), 0.0f,
               0.0f, 0.0f, 1.0f
            });
}
