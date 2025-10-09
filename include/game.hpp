#ifndef __GAME__
#define __GAME__

#include "entt.hpp"
#include "raylib.h"

#include "resources.hpp"
#include "utils.hpp"
#include <vector>

namespace Game {

struct GameContext {
    entt::registry registry;
    ResourceManager resourceManager;

    Camera2D camera;
};

void InitContext(GameContext& ctx);
void InitGame(GameContext& ctx);

void Update(GameContext& ctx);
void Render(GameContext& ctx);

} // GameContext namespace

#endif