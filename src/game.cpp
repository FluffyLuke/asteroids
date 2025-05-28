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

std::optional<EntityID> Entity::GetEntityByName(GameContext* ctx, std::string name) {
    for(auto& e : ctx->entities) {
        if(Entity::GetEntityComponent(ctx, e.first)->name == name) {
            return std::make_optional(e.first);
        }
    }
    return std::nullopt;
}

EntityID Entity::New(GameContext* ctx) {
    EntityID nextID = index;

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);
    ec->parent = std::nullopt;

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    ctx->topEntities.push_back(nextID);

    index++;
    return nextID;
}

EntityID Entity::New(GameContext* ctx, EntityComponent* parent) {
    EntityID nextID = index;

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);
    ec->parent = std::make_optional(parent->GetID());

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    parent->children.push_back(nextID);
    index++;
    return nextID;
}

EntityID Entity::New(GameContext* ctx, EntityID parent_id) {
    EntityID nextID = index;
    EntityComponent* parent = GetEntityComponent(ctx, parent_id);

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);

    ec->parent = std::make_optional(parent->GetID());

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    parent->children.push_back(nextID);
    index++;
    return nextID;
}

EntityID Entity::New(GameContext* ctx, std::string name) {
    EntityID e = Entity::New(ctx);
    Entity::GetEntityComponent(ctx, e)->name = name;
    return e;
}

EntityID Entity::New(GameContext* ctx, EntityID parent_id, std::string name) {
    EntityID e = Entity::New(ctx, parent_id);
    Entity::GetEntityComponent(ctx, e)->name = name;
    return e;
}

EntityID Entity::New(GameContext* ctx, EntityComponent* parent, std::string name) {
    EntityID e = Entity::New(ctx, parent);
    Entity::GetEntityComponent(ctx, e)->name = name;
    return e;
}

void Entity::Destroy(GameContext* ctx, EntityID id) {
    ctx->deadEntities.push_back(id);
    EntityComponent* ec = Entity::GetEntityComponent(ctx, id);
    if(ec->parent.has_value()) {
        EntityComponent* parent_ec = GetEntityComponent(ctx, ec->parent.value());
        parent_ec->children.push_back(parent_ec->GetID());
    }
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