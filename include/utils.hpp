#ifndef __UTILS__
#define __UTILS__

#include <cstdint>

#include <raylib.h>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

using u8 = uint16_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using radian = float;
using degree = float;

// Raylib-specific utils

struct Vector2Int {
    int x;
    int y;
};
Vector2Int VectorToIntVector(Vector2 v);

Vector2Int GetScreenDimensions();

#endif