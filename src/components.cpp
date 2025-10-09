#include "raylib.h"

#include "components.hpp"
#include "utils.hpp"

Rectangle C::GetCurrentSprite2DPart(Sprite2D& sprite) {
    return sprite.parts[sprite.currentPartIndex];
}