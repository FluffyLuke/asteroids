#include <spdlog/spdlog.h>
#include <memory>

#include "raylib.h"
#include "raymath.h"
#include "game.hpp"

// General interface initialization

IComponent::IComponent(GameContext* ctx, GameObject* gameObject) {
    this->ctx = ctx;
    this->gameObject = gameObject;
}

// === Game Manager ===

GameManager::GameManager(GameContext* ctx, GameObject* gameObject): IComponent(ctx, gameObject) {
    instance = this;
}
void GameManager::CreateGameManager(GameContext* ctx) {
    auto gm = ctx->NewGameObject();

    auto gmc = std::make_unique<GameManager>(ctx, gm);
    gm->AddComponent(std::move(gmc));
}

PlayerControllerComponent* GameManager::spawnPlayer() {
    auto player = ctx->NewGameObject();
    player->scale = 1.2;

    auto pcc = std::make_unique<PlayerControllerComponent>(ctx, player);
    auto rc = std::make_unique<RenderComponent>(ctx, player);

    PlayerControllerComponent* player_controller_raw = pcc.get();

    player->AddComponent(std::move(pcc));
    player->AddComponent(std::move(rc));

    return player_controller_raw;
}

GameManager* GameManager::instance = nullptr;
GameManager* GameManager::getInstance() {
    return instance;
}

GameState GameManager::GetGameState() {
    return state;
}

void GameManager::Start() {
    state = MainMenu;

    auto player = spawnPlayer();

    Vector2 middle = ScreenCenter();

    player->gameObject->pos = middle;
}
void GameManager::Update() {
    
}
void GameManager::End() {

}

// === UI Manager ===

UIManager::UIManager(GameContext* ctx, GameObject* gameObject): IComponent(ctx, gameObject) {}
void UIManager::CreateUIManager(GameContext *ctx) {
    auto uim = ctx->NewGameObject();

    // auto uimc = std::make_unique<UIManager>(ctx, uim);
    // uim->AddComponent(std::move(uimc));
}

void UIManager::renderStartScreen() {
    std::string title = "Asteroids!";

    float spacing = 5;
    float fontSize = 25;
    Vector2 pos = ScreenCenter(); pos.y = 20;
    Vector2 origin = MeasureTextEx(GetFontDefault(), title.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        title.c_str(), 
        pos, origin, 
        0, 
        fontSize, spacing, RED);
}

void UIManager::renderGameScreen() {

}

void UIManager::renderEndScreen() {

}

void UIManager::Start() {
    spdlog::info("Dupsko");
}

void UIManager::Update() {
    // GameManager* gm = GameManager::getInstance();
    // switch(gm->GetGameState()) {
    //     case MainMenu: renderStartScreen(); break;
    //     case Game: renderGameScreen(); break;
    //     case GameOver: renderEndScreen(); break;
    // }
}

void UIManager::End() {
    
}

// === Player Component ===

PlayerControllerComponent::PlayerControllerComponent(GameContext* ctx, GameObject* gameObject): IComponent(ctx, gameObject) {}
void PlayerControllerComponent::Update() {

    // Movement
    {
        Vector2 moveVector = {0,0};

        // Rotate player
        if(IsKeyDown(KEY_A)) {
            gameObject->rotation -= 200 * GetFrameTime();
        }
        if(IsKeyDown(KEY_D)) {
            gameObject->rotation += 200 * GetFrameTime();
        }

        if(IsKeyDown(KEY_W)) {
            moveVector.x += 400 * GetFrameTime();
        }

        float rotation = DEG2RAD * gameObject->rotation;

        moveVector = Vector2Rotate(moveVector, rotation);
        gameObject->pos.x += moveVector.x;
        gameObject->pos.y += moveVector.y;

        spdlog::debug("Player movement:");
        spdlog::debug("Player position: x{}, y{}", gameObject->pos.x, gameObject->pos.y);
        spdlog::debug("Player rotation: {}", gameObject->rotation);
        spdlog::debug("Move vector: x{}, y{}", moveVector.x, moveVector.y);
    }

    // Check if out of bounds
    {
        Rectangle screenArea = {
            0,
            0, 
            static_cast<float>(GetScreenWidth()), 
            static_cast<float>(GetScreenWidth())
        };

      if(!InsideRec(screenArea, gameObject->pos)) {
        gameObject->pos = ScreenCenter();
      }
    ;}
}

// === Render Component ===

RenderComponent::RenderComponent(GameContext* ctx, GameObject* gameObject): IComponent(ctx, gameObject) {}
void RenderComponent::Update() {
    std::optional<Texture2DResource> result = ctx->resourceManager->GetTexture(texture_names::PlayerTexture);
    if(result.has_value()) {
        Texture2DResource texture = result.value();

        Vector2 pos = gameObject->pos;
        Vector2 destSize = {texture.GetFrameSize().x * gameObject->scale, texture.GetFrameSize().y * gameObject->scale};
        Vector2 origin;
        origin.x = destSize.x/2;
        origin.y = destSize.y/2;

        DrawTexturePro(
            texture.texture, 
            texture.GetFrame(0), 
            {pos.x, pos.y, destSize.x,destSize.y}, 
            origin, 
            gameObject->rotation + 90, 
            WHITE
            );
    } else {
        spdlog::error("Cannot load texture!");
    }
}