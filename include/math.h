#ifndef MATH_H
#define MATH_H
#include "include/types.h"

struct Matrix3f {
    float data[9];

    Matrix3f operator*(const Matrix3f& other) const;
    V2 operator*(const V2& other) const;
};

#endif //MATH_H
