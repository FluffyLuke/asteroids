#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>

#include "raylib.h"
#include "game.hpp"

using namespace std;

void LoadAllResources(GameContext* ctx) {
    ctx->resourceManager->LoadTexture(texture_names::PlayerTexture, {30,30}, {1,1}, "./assets/player_placeholder.png");
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        spdlog::info("Debug mode: on");
        spdlog::set_level(spdlog::level::trace);
    }


    GameContext ctx{};

    InitWindow(720, 600, "Asteroids");
    SetTargetFPS(60);

    // Create UI manager before game manager. Otherwise program will explode, for reasons 
    UIManager::CreateUIManager(&ctx);
    GameManager::CreateGameManager(&ctx);

    LoadAllResources(&ctx);

    spdlog::info("Starting game!");
    for(auto&& go : ctx.gameObjects) {
        for(auto&& c : go->components) {
            c->Start();
        }
    }
    while(!WindowShouldClose()) {
        spdlog::debug("=== New iteration ===");
        spdlog::debug("Screen size: x{}, y{}", GetScreenWidth(), GetScreenHeight());
        BeginDrawing();
        ClearBackground(BLACK);

        for(auto&& gm : ctx.gameObjects) {
            for(auto&& c : gm->components) {
                c->Update();
            }
        }

        for(auto&& gm : ctx.gameObjects) {
            for(auto&& c : gm->components) {
                c->LateUpdate();
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