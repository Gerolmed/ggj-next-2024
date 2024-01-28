#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <assert.h>
#include <glm/mat4x4.hpp>

#define DEBUG
#define WINDOWS
// #define FULLSCREEN

#ifdef WINDOWS
#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#endif
#endif

#define PI 3.141592653f
#define degreesToRadians(angleDegrees) ((angleDegrees) * 3.141592653f / 180.0f)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0f / 3.141592653f)

struct V2 
{
    float x;
    float y;

    V2 operator+(const V2& v2) const;
    V2 operator-(const V2& v2) const;
    V2 operator*(const float& value) const;
    float Magnitude();
    float SqrMagnitude();
    float ToRad();
    V2 Norm();
};

inline V2 v2(float x)
{
    return {x, x};
}

inline V2 v2(float x, float y)
{
    return {x, y};
}

union V3
{
    struct {
        float x;
        float y;
        float z;
    };

    V2 xy;
};

inline V3 v3(float x)
{
    return {x, x, x};
}

inline V3 v3(float x, float y, float z)
{
    return {x, y, z};
}

struct Sprite {
    V2 bottom_left;
    V2 top_right;
};

inline Sprite sprite(V2 bottom_left, V2 top_right)
{
    return {bottom_left, top_right};
}

struct Mat2f {
    float data[4];

    Mat2f operator*(const Mat2f& other) const;
    V2 operator*(const V2& other) const;
};

inline Mat2f mat2(float f) 
{
    return Mat2f { f, 0, 0, f };
}

Mat2f rotationMatrix2f(float angle);

struct Mat3f {
    float data[9];

    Mat3f operator*(const Mat3f& other) const;
    V3 operator*(const V3& other) const;
    V2 operator*(const V2& other) const;
};

Mat3f translationMatrix(V2 v2);
Mat3f rotationMatrix3f(float angle);
Mat3f rotationAndTranslationMatrix(V2 v2, float angle);

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef glm::mat4 Mat4;

#endif
