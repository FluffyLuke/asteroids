#include "raylib.h"
#include "spdlog/spdlog.h"
#include "game.hpp"

#include <memory>

// Game context
GameContext::GameContext() 
    : resourceManager(std::make_unique<ResourceManager>()) {}
GameContext::~GameContext() = default;

// === Utils ===

Vector2 ScreenCenter() {
    return {
        GetScreenWidth() / 2.0f,
        GetScreenHeight() / 2.0f,
    };
}

bool InsideRec(Rectangle rec, Vector2 point) {

    bool a = point.x > rec.x;
    bool b = point.y > rec.y;
    bool c = point.x < rec.x + rec.width;
    bool d = point.y < rec.y + rec.height;

    return (a && b && c && d);
}