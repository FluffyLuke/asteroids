#include "raylib.h"
#include "game.hpp"

#include <memory>
#include <vector>


// Game object
GameObject::GameObject(GameContext* ctx) {
    this->ctx = ctx;
    pos = {0, 0};
}
void GameObject::AddComponent(std::unique_ptr<IComponent> component) {
    components.push_back(std::move(component));
}
template<class TComponent>
void GameObject::RemoveComponent() {
    std::erase_if(components, [](const std::unique_ptr<IComponent>& c) {
        return dynamic_cast<TComponent*>(c.get()) != nullptr;
    });
}

// Game context
GameObject* GameContext::NewGameObject() {
    auto gm = std::make_unique<GameObject>(this);
    GameObject* gmRaw = gm.get();
    gameObjects.push_back(std::move(gm));
    return gmRaw;
}

GameContext::GameContext() 
    : resourceManager(std::make_unique<ResourceManager>()) {}
GameContext::~GameContext() = default;