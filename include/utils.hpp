#ifndef __UTILS__
#define __UTILS__

#include <cstdint>
#include "raylib.h"

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

Vector2 ScreenCenter();
Vector2 RandomPointOnCircleEdge(Vector2 center, f32 radius);

bool InsideRec(Rectangle rec, Vector2 point);

struct Counter {
    f32 value;
    f32 maxValue;

    Counter(f32 maxValue);
    Counter(f32 minValue, f32 maxValue);

    void ResetCounter();
    bool UpdateCounter(f32 v);
    bool Check();
};

#endif