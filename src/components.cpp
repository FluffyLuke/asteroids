#include <spdlog/spdlog.h>
#include <memory>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "game.hpp"

// General interface initialization

IComponent::IComponent(GameContext* ctx, EntityID id) {
    this->ctx = ctx;
    this->id = id;
}

EntityID IComponent::GetID() {
    return id;
}

// === Entity Component ===

EntityComponent::EntityComponent(GameContext* ctx, EntityID id) : IComponent(ctx, id) {
    Start();
}
void EntityComponent::Start() {}

// === Game Manager ===

GameManager::GameManager(GameContext* ctx, EntityID id): IComponent(ctx, id) {
    //printf("this->ctx = %p\n", this->ctx);
    this->instance = this;
    this->instance->ctx = ctx;
    Start();
}
void GameManager::CreateGameManager(GameContext* ctx) {
    EntityID gm = Entity::New(ctx);

    auto gmc = std::make_unique<GameManager>(ctx, gm);
    Entity::AddComponent(ctx, gm, std::move(gmc));
}

void GameManager::ReceiveData(PlayerEvent::PlayerEvent e) {
    if(e == PlayerEvent::PlayerDied) {
        state = GameOver;
    }
}

PlayerControllerComponent* GameManager::spawnPlayer() {
    std::optional entity_mainGame = Entity::GetEntityByName(ctx, "MainGame");

    EntityID player = Entity::New(ctx, entity_mainGame.value());
    EntityComponent* playerEntity = Entity::GetEntityComponent(ctx, player);

    playerEntity->scale = {1.2, 1.2};

    auto pcc = std::make_unique<PlayerControllerComponent>(ctx, player);
    auto rc = std::make_unique<RenderComponent>(ctx, player);

    PlayerControllerComponent* playerControllerRaw = pcc.get();
    playerControllerRaw->RegisterSubscriber(*this);

    Entity::AddComponent(ctx, player, std::move(pcc));
    Entity::AddComponent(ctx, player, std::move(rc));

    Vector2 middle = ScreenCenter();
    playerEntity->pos = middle;

    return playerControllerRaw;
}

GameManager* GameManager::instance = nullptr;
GameManager* GameManager::getInstance() {
    return instance;
}

GameState GameManager::GetGameState() {
    return state;
}

void GameManager::StartGame() {
    spawnPlayer();
    state = Game;
    timeAlive = 0;
}

void GameManager::Start() {
    state = MainMenu;
}

void GameManager::Update() {
    timeAlive += GetFrameTime();
}

void GameManager::End() {
    
}

// === UI Manager ===

UIManager::UIManager(GameContext* ctx, EntityID id): IComponent(ctx, id) {
    Start();
}
void UIManager::CreateUIManager(GameContext *ctx) {

    std::optional entity_ui = Entity::GetEntityByName(ctx, "UI");
    auto uim = Entity::New(ctx, entity_ui.value());

    auto uimc = std::make_unique<UIManager>(ctx, uim);
    Entity::AddComponent(ctx, uim, std::move(uimc));
}

void UIManager::renderStartScreen() {
    std::string title = "Asteroids!";

    float spacing = 10;
    float fontSize = 100;
    Vector2 pos = ScreenCenter(); pos.y = 75;
    Vector2 origin = MeasureTextEx(GetFontDefault(), title.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        title.c_str(), 
        pos, origin, 
        0, 
        fontSize, spacing, RED
    );

    std::string startText = "Press \"space\" to start";

    fontSize = 24;
    pos = ScreenCenter();
    origin = MeasureTextEx(GetFontDefault(), startText.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        startText.c_str(), 
        pos, origin, 
        0, 
        fontSize, spacing, RED
    );
}

void UIManager::renderGameScreen() {
    GameManager* gm = GameManager::getInstance();

    std::string time = std::to_string((int)gm->timeAlive);

    float spacing = 10;
    float fontSize = 48;
    Vector2 pos {
        ScreenCenter().x,
        GetScreenHeight() - 30.0f,
    };
    Vector2 origin = MeasureTextEx(GetFontDefault(), time.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        time.c_str(), 
        pos, origin, 
        0, 
        fontSize, spacing, RED
    );
}

void UIManager::renderEndScreen() {
    std::string gameOverText = "Game Over";
    std::string tryAgainText = "Press \"Space\" to try again";

    float spacing = 10;
    float fontSize = 100;
    Vector2 pos = ScreenCenter();
    Vector2 origin = MeasureTextEx(GetFontDefault(), gameOverText.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        gameOverText.c_str(), 
        pos, origin, 
        0, 
        fontSize, spacing, RED
    );

    fontSize = 48;
    pos.y += 60;
    origin = MeasureTextEx(GetFontDefault(), tryAgainText.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;
}

void UIManager::Start() {
    
}

void UIManager::Update() {
    GameManager* gm = GameManager::getInstance();
    switch(gm->GetGameState()) {
        case MainMenu: {
            renderStartScreen();
            if(IsKeyPressed(KEY_SPACE)) {
                gm->StartGame();
            }
        } break;
        case Game: renderGameScreen(); break;
        case GameOver: {
            renderEndScreen();
            if(IsKeyPressed(KEY_SPACE)) {
                gm->StartGame();
            }
        } break;
    }
}

void UIManager::End() {
    
}

// === Player Component ===

PlayerControllerComponent::PlayerControllerComponent(GameContext* ctx, EntityID id): IComponent(ctx, id) {
    Start();
}

void PlayerControllerComponent::Die() {
    PublishData(PlayerEvent::PlayerDied);
    Entity::Destroy(ctx, id);
}

void PlayerControllerComponent::Update() {
    EntityComponent* playerEntity = Entity::GetEntityComponent(ctx, id);

    // Movement
    {
        Vector2 moveVector = {0,0};

        // Rotate player
        if(IsKeyDown(KEY_A)) {
            playerEntity->rotation -= 200 * GetFrameTime();
        }
        if(IsKeyDown(KEY_D)) {
            playerEntity->rotation += 200 * GetFrameTime();
        }

        if(IsKeyDown(KEY_W)) {
            moveVector.x += 400 * GetFrameTime();
        }

        float rotation = DEG2RAD * playerEntity->rotation;

        moveVector = Vector2Rotate(moveVector, rotation);
        playerEntity->pos.x += moveVector.x;
        playerEntity->pos.y += moveVector.y;

        spdlog::debug("- Player movement -");
        spdlog::debug("Player position: x{}, y{}", playerEntity->pos.x, playerEntity->pos.y);
        spdlog::debug("Player rotation: {}", playerEntity->rotation);
        spdlog::debug("Move vector: x{}, y{}", moveVector.x, moveVector.y);
    }

    // Check if out of bounds
    {
        Rectangle screenArea = {
            0,
            0, 
            static_cast<float>(GetScreenWidth()), 
            static_cast<float>(GetScreenHeight())
        };

      if(!InsideRec(screenArea, playerEntity->pos)) {
        Die();
      }
    }
}

// === Render Component ===

RenderComponent::RenderComponent(GameContext* ctx, EntityID id): IComponent(ctx, id) {
    Start();
}

void RenderComponent::Update() {
    EntityComponent* entity = Entity::GetEntityComponent(ctx, id);

    std::optional<Texture2DResource> result = ctx->resourceManager->GetTexture(texture_names::PlayerTexture);
    if(result.has_value()) {
        Texture2DResource texture = result.value();

        Vector2 pos = entity->pos;
        Vector2 destSize = {texture.GetFrameSize().x * entity->scale.x, texture.GetFrameSize().y * entity->scale.y};
        Vector2 origin;
        origin.x = destSize.x/2;
        origin.y = destSize.y/2;

        DrawTexturePro(
            texture.texture, 
            texture.GetFrame(0), 
            {pos.x, pos.y, destSize.x,destSize.y}, 
            origin, 
            entity->rotation + 90, 
            WHITE
            );
    } else {
        spdlog::error("Cannot load texture!");
    }
}