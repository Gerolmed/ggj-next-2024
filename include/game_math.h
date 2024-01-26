#ifndef GAME_MATH_HPP
#define GAME_MATH_HPP

inline float clamp(float value, float min, float max) 
{
    if (value >= max) 
        return max;
    else if (value <= min)
        return min;
    return value;
}

inline float min(float a, float b) 
{
    return a > b ? b : a;
}

inline u8 collides(float ax, float ay, float aw, float ah,
                      float bx, float by, float bw, float bh)
{
    return ax - aw / 2 < bx + bw / 2 &&
            ax + aw / 2 > bx - bw / 2 &&
            ay - ah / 2 < by + bh / 2 &&
            ay + ah / 2 > by - bh / 2;
}

inline float absf(float f) 
{
    return f > 0 ? f : -f;
}

inline u32 max(u32 a, u32 b)
{
    return a > b? a : b;
}

#endif
