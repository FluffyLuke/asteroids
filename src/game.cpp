#include "raylib.h"
#include "spdlog/spdlog.h"
#include "game.hpp"

#include <memory>
#include <optional>
#include <stdexcept>

// Game object

i64 Entity::index = 0;

void Entity::AddComponent(GameContext* ctx, EntityID id, std::unique_ptr<IComponent> component) {
    try {
        auto& components = ctx->entities.at(id);
        components.push_back(std::move(component));
    } catch(std::out_of_range& e) {
        spdlog::error("Entity ID {} not found: {}", id, e.what());
    }
}

EntityComponent* Entity::GetEntityComponent(GameContext* ctx, EntityID id) {
    return Entity::FindComponent<EntityComponent>(ctx, id).value();
}

template<typename T>
std::optional<T*> Entity::FindComponent(GameContext* ctx, EntityID id) {
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

EntityID Entity::New(GameContext* ctx) {
    EntityID nextID = index;

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::make_unique<EntityComponent>(ctx, nextID));

    ctx->entities.insert({nextID, std::move(components)});
    index++;
    return nextID;
}

void Entity::Destroy(GameContext* ctx, EntityID id) {
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