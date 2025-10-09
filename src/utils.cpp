#include "raylib.h"

#include "utils.hpp"

// Raylib-specific utils

Vector2Int VectorToIntVector(Vector2 v) {
    return {
        static_cast<int>(v.x),
        static_cast<int>(v.y)
    };
}

Vector2Int GetScreenDimensions() {
    return {
        GetScreenWidth(),
        GetScreenHeight()
    };
}