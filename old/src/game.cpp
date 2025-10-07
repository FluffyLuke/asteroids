#include "raylib.h"
#include "spdlog/spdlog.h"
#include "game.hpp"

#include <memory>

// Game context
GameContext::GameContext() 
    : resourceManager(std::make_unique<ResourceManager>()) {}
GameContext::~GameContext() = default;