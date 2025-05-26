#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>

#include "raylib.h"
#include "game.hpp"

using namespace std;

void LoadAllResources(GameContext* ctx) {
    ctx->resourceManager->LoadTexture(texture_names::PlayerTexture, {30,30}, {1,1}, "./assets/player_placeholder.png");
}

int main() {
    GameContext ctx{};

    InitWindow(720, 600, "Asteroids");
    SetTargetFPS(60);

    UIManager::CreateUIManager(&ctx);
    GameManager::CreateGameManager(&ctx);

    LoadAllResources(&ctx);

    spdlog::info("Starting game!");
    for(auto&& go : ctx.gameObjects) {
        for(auto&& c : go->components) {
            c->Start();
            spdlog::info("Created");
        }
    }
    while(!WindowShouldClose()) {
        spdlog::debug("=== New iteration ===");
        BeginDrawing();
        ClearBackground(BLACK);

        for(auto&& gm : ctx.gameObjects) {
            for(auto&& c : gm->components) {
                c->Update();
            }
        }

        EndDrawing();
    }
    for(auto&& gm : ctx.gameObjects) {
        for(auto&& c : gm->components) {
            c->End();
        }
    }

    CloseWindow();
    
    return 0;
}