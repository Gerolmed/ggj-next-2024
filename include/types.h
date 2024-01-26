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


struct V2 
{
    float x;
    float y;
};

inline V2 v2(float x)
{
    return {x, x};
}

inline V2 v2(float x, float y)
{
    return {x, y};
}

struct V3
{
    float x;
    float y;
    float z;
};

inline V3 v3(float x)
{
    return {x, x, x};
}

inline V3 v3(float x, float y, float z)
{
    return {x, y, z};
}

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
