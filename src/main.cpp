#include <memory>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>

#include "raylib.h"
#include "game.hpp"

void LoadAllResources(GameContext* ctx) {
    ctx->resourceManager->LoadTexture(texture_names::PlayerTexture, {30,30}, {1,1}, "./assets/player_placeholder.png");
}

void CreateTopLevelObjects(GameContext* ctx) {
    Entity::New(ctx, "MainGame");
    Entity::New(ctx, "UI");
}

int main(int argc, char ** argv) {
    if(argc > 1) {
        spdlog::info("Debug mode: on");
        spdlog::set_level(spdlog::level::trace);
    }

    GameContext ctx{};

    InitWindow(720, 600, "Asteroids");
    SetTargetFPS(5);

    // Make top level objects 
    CreateTopLevelObjects(&ctx);

    UIManager::CreateUIManager(&ctx);
    GameManager::CreateGameManager(&ctx);

    LoadAllResources(&ctx);

    while(!WindowShouldClose()) {
        spdlog::debug("=== New iteration ===");
        spdlog::debug("Screen size: x{}, y{}", GetScreenWidth(), GetScreenHeight());
        BeginDrawing();
        ClearBackground(BLACK);

        std::vector<EntityID> currentEntities;
        for(EntityID id : ctx.topEntities) {
            //spdlog::info("INSERTING ENTITY!");
            currentEntities.push_back(id);
        }

        while(!currentEntities.empty()) {
            EntityID current = currentEntities.back();
            currentEntities.pop_back();

            auto& components = ctx.entities.at(current);
            for(auto&& c : components) {
                c->Update();
            }
            for(EntityID childID : Entity::GetEntityComponent(&ctx, current)->children) {
                currentEntities.push_back(childID);
            }
        }

        for(auto&& e : ctx.deadEntities) {
            auto& components = ctx.entities.at(e);
            for(auto&& c : components) {
                c->End();
            }
            Entity::RemoveParent(&ctx, e);
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