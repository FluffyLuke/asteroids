#include <string>

#include "raylib.h"

#include "game.hpp"
#include "utils.hpp"

i32 main(int argc, char** args) {
    constexpr i32 screenWidth = 1080;
    constexpr i32 screenHeight = 720;
    std::string title = "Asteroids";

    Game::GameContext ctx;
    Game::InitContext(ctx);

    Game::InitGame(ctx);

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground({0,0,0,1});
            BeginMode2D(ctx.camera);
                Game::Update(ctx);
                Game::Render(ctx);
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}