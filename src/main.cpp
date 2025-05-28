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
    for(auto&& e : ctx.entities) {
        for(auto&& c : e.second) {
            c->Start();
        }
    }
    while(!WindowShouldClose()) {
        spdlog::debug("=== New iteration ===");
        spdlog::debug("Screen size: x{}, y{}", GetScreenWidth(), GetScreenHeight());
        BeginDrawing();
        ClearBackground(BLACK);

        for(auto&& e : ctx.entities) {
            for(auto&& c : e.second) {
                c->Update();
            }
        }

        for(auto&& e : ctx.entities) {
            for(auto&& c : e.second) {
                c->LateUpdate();
            }
        }

        for(auto&& e : ctx.deadEntities) {
            auto& components = ctx.entities.at(e);
            for(auto&& c : components) {
                c->End();
            }
            ctx.entities.erase(e);
        }
        ctx.deadEntities.clear();

        EndDrawing();
    }
    for(auto&& gm : ctx.entities) {
        for(auto&& c : gm.second) {
            c->End();
        }
    }

    CloseWindow();
    
    return 0;
}