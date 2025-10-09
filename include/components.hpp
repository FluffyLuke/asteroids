#ifndef __COMPONENTS__
#define __COMPONENTS__

#include <vector>

#include "raylib.h"
#include "resources.hpp"
#include "utils.hpp"

namespace C {

struct Transform2D {
    Vector2 position;
    degree rotation;
};

struct Sprite2D {
    ResourceID textureID;
    Vector2Int textureSize;

    std::vector<Rectangle> parts;
    u32 currentPartIndex;
};

Rectangle GetCurrentSprite2DPart(Sprite2D& sprite);

struct Player {
    
};

} // Component namespace

#endif