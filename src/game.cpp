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
    bool d = point.x < rec.y + rec.height;

    return (a && b && c && d);
}