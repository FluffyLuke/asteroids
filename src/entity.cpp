#include "spdlog/spdlog.h"
#include "game.hpp"

#include <memory>
#include <optional>
#include <stdexcept>

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

std::optional<EntityID> Entity::GetEntityByName(GameContext* ctx, std::string name) {
    for(auto& e : ctx->entities) {
        if(Entity::GetEntityComponent(ctx, e.first)->name == name) {
            return std::make_optional(e.first);
        }
    }
    return std::nullopt;
}

void Entity::SetParent(GameContext* ctx, EntityID child_id, EntityID newParent_id) {
    EntityComponent* c = Entity::GetEntityComponent(ctx, child_id);

    // Remove previous child from its parent
    Entity::RemoveParent(ctx, child_id);

    c->parent = std::make_optional(newParent_id);
    Entity::GetEntityComponent(ctx, newParent_id)->children.push_back(child_id);
}

// TODO move to better vectors, standard ones are shit
bool Entity::RemoveParent(GameContext* ctx, EntityID child_id) {
    EntityComponent* child = Entity::GetEntityComponent(ctx, child_id);
    EntityID parent_id = child->parent.value();
    EntityComponent* parent = Entity::GetEntityComponent(ctx, parent_id);
    if (parent) {
        auto& children = parent->children;
        children.erase(std::remove(children.begin(), children.end(), child_id), children.end());
    }
    child->parent = std::nullopt;

    return false;
}

EntityID Entity::New(GameContext* ctx) {
    EntityID nextID = index;
    index++;

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);
    ec->parent = std::nullopt;

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    spdlog::info("Inserted new object of id: {}", nextID);
    ctx->newEntities.push_back(nextID);
    ctx->topEntities.push_back(nextID);

    return nextID;
}

EntityID Entity::New(GameContext* ctx, EntityComponent* parent) {
    EntityID nextID = index;
    index++;

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);
    ec->parent = std::make_optional(parent->GetID());

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    spdlog::info("Inserted new object of id: {}", nextID);
    ctx->newEntities.push_back(nextID);
    parent->children.push_back(nextID);

    return nextID;
}

EntityID Entity::New(GameContext* ctx, EntityID parent_id) {
    EntityID nextID = index;
    index++;

    EntityComponent* parent = GetEntityComponent(ctx, parent_id);

    std::unique_ptr<EntityComponent> ec = std::make_unique<EntityComponent>(ctx, nextID);

    ec->parent = std::make_optional(parent->GetID());

    std::list components = std::list<std::unique_ptr<IComponent>>();
    components.push_back(std::move(ec));

    ctx->entities.insert({nextID, std::move(components)});
    spdlog::info("Inserted new object of id: {}", nextID);
    ctx->newEntities.push_back(nextID);
    parent->children.push_back(nextID);
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
}