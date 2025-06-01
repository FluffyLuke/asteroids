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

Vector2 RandomPointOnCircleEdge(Vector2 center, f32 radius) {
    float randomAngle = GetRandomValue(0, 360);
    // degrees to radians
    randomAngle *= PI / 180;
    float edgeX = center.x + radius * cosf(randomAngle);
    float edgeY = center.y + radius * sinf(randomAngle);

    return {edgeX, edgeY};
}

bool InsideRec(Rectangle rec, Vector2 point) {

    bool a = point.x > rec.x;
    bool b = point.y > rec.y;
    bool c = point.x < rec.x + rec.width;
    bool d = point.y < rec.y + rec.height;

    return (a && b && c && d);
}