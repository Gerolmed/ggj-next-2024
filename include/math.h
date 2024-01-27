#ifndef MATH_H
#define MATH_H
#include "include/types.h"

struct Mat2f {
    float data[4];

    Mat2f operator*(const Mat2f& other) const;
    V2 operator*(const V2& other) const;
};

Mat2f rotationMatrix2f(float angle);

struct Mat3f {
    float data[9];

    Mat3f operator*(const Mat3f& other) const;
    V3 operator*(const V3& other) const;
};



Mat3f translationMatrix(V2 v2);
Mat3f rotationMatrix3f(float angle);
Mat3f rotationAndTranslationMatrix(V2 v2, float angle);




#endif //MATH_H
