#include "raylib.h"
#include "spdlog/spdlog.h"
#include "game.hpp"

#include <memory>
#include <optional>
#include <stdexcept>


// Game object

i64 GameObject::index = 0;

template<typename T>
void GameObject::AddComponent(GameContext* ctx, EntityID id, T component) {
    try {
        auto components = ctx->entities.at(id);
        components.push_back(component);
    } catch(std::out_of_range e) {
        spdlog::error("Entity ID {} not found: {}", id, e.what());
    }
}

template<typename T>
std::optional<T*> GameObject::FindComponent(GameContext* ctx, EntityID id) {
    try {
        auto& components = ctx->entities.at(id);
        for(auto&& c : components) {
            if(auto ptr = dynamic_cast<T*>(c.get())) {
                return std::optional<T*>{ptr};
            }
        }
    } catch(const std::out_of_range& e) {
        spdlog::error("Entity ID {} not found: {}", id, e.what());
    }

    return std::nullopt;
}

EntityID GameObject::New(GameContext* ctx) {
    EntityID nextID = index;
    ctx->entities.insert({nextID, std::list<std::unique_ptr<IComponent>>()});
    index++;
    return nextID;
}

void GameObject::Destroy(GameContext* ctx, EntityID id) {
    ctx->deadEntities.push_back(id);
}

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