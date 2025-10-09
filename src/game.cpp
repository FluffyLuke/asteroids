#include "components.hpp"
#include "raylib.h"
#include "resources.hpp"
#include "game.hpp"

void Game::InitContext(Game::GameContext& ctx){   
    Vector2Int dimensions = GetScreenDimensions();
    ctx.camera = { 0 };
    ctx.camera.target = { 0, 0 };
    ctx.camera.offset = { dimensions.x / 2.0f, dimensions.y / 2.0f };
    ctx.camera.rotation = 0.0f;
    ctx.camera.zoom = 1.0f;
};

void Game::InitGame(GameContext &ctx) {
    
}

void Game::Update(Game::GameContext& ctx) {
    auto view = ctx.registry.view<C::Transform2D, C::Player>();
    
    // use an extended callback
    // view.each([](const auto entity, const auto &pos, auto &vel) {

    // });
}

void Game::Render(Game::GameContext& ctx) {
    auto view = ctx.registry.view<C::Transform2D, C::Sprite2D>();

    view.each([&ctx](const auto entity, const C::Transform2D& trans, C::Sprite2D& sprite) {
        Rectangle s = C::GetCurrentSprite2DPart(sprite);
        Rectangle dest = {
            trans.position.x,
            trans.position.y,
            s.width,
            s.height
        };
        Vector2 origin = {
            s.width / 2,
            s.height / 2
        };

        Texture2DResource res = ctx.resourceManager.GetTexture2DResource(sprite.textureID);

        DrawTexturePro(res.texture, s, dest, origin, trans.rotation, WHITE);
    });
}