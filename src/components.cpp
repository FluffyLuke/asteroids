#include <spdlog/spdlog.h>
#include <memory>
#include <string>

#include "raylib.h"
#include "raymath.h"
#include "game.hpp"
#include "utils.hpp"

// General interface initialization

IComponent::IComponent(GameContext* ctx, EntityID id) {
    this->ctx = ctx;
    this->id = id;
}

EntityID IComponent::GetID() {
    return id;
}

// === Entity Component ===

EntityComponent::EntityComponent(GameContext* ctx, EntityID id) : IComponent(ctx, id) {}
void EntityComponent::Start() {}

// === Collider Component ===

CircleColliderComponent::CircleColliderComponent(GameContext* ctx, EntityID id) : IComponent(ctx, id) {}

void CircleColliderComponent::SetCollider(f32 radius) {
    this->radius = radius;
}

void CircleColliderComponent::Update() {
    EntityComponent* ec = Entity::GetEntityComponent(ctx, id);

    auto colliders = Entity::FindOtherComponents<CircleColliderComponent>(ctx, id);

    for(auto c : colliders) {
        EntityComponent* other = Entity::GetEntityComponent(ctx, c->GetID());
        f32 distance = Vector2Distance(ec->pos, other->pos);
        f32 totalRadius = this->radius + c->radius;
        if(distance < totalRadius) {
            spdlog::info("Hit");
            c->PublishData({other->GetID()});
        }
    };
}

// === Game Manager ===

GameManager::GameManager(GameContext* ctx, EntityID id) : IComponent(ctx, id) {
    //printf("this->ctx = %p\n", this->ctx);
    this->instance = this;
    this->instance->ctx = ctx;
}

void GameManager::CreateGameManager(GameContext* ctx) {
    EntityID gm = Entity::New(ctx);

    auto gmc = std::make_unique<GameManager>(ctx, gm);
    Entity::AddComponent(ctx, gm, std::move(gmc));
}

void GameManager::ReceiveData(PlayerEvent e) {
    if(e == PlayerEvent::PlayerDied) {
        state = GameOver;
        for(auto c : Entity::FindComponents<AsteroidComponent>(ctx)) {
            Entity::Destroy(ctx, c->GetID());
        }
    }
}

PlayerControllerComponent* GameManager::spawnPlayer() {
    std::optional entity_mainGame = Entity::GetEntityByName(ctx, "MainGame");

    // Player
    EntityID player = Entity::New(ctx, entity_mainGame.value());
    EntityComponent* playerEntity = Entity::GetEntityComponent(ctx, player);

    playerEntity->scale = {1.2, 1.2};

    auto pcc = std::make_unique<PlayerControllerComponent>(ctx, player);
    auto rc = std::make_unique<RenderComponent>(ctx, player);
    auto cc = std::make_unique<CircleColliderComponent>(ctx, player);
    cc->SetCollider(20);

    PlayerControllerComponent* playerControllerRaw = pcc.get();
    playerControllerRaw->RegisterSubscriber(this);

    Entity::AddComponent(ctx, player, std::move(pcc));
    Entity::AddComponent(ctx, player, std::move(rc));
    Entity::AddComponent(ctx, player, std::move(cc));

    Vector2 middle = ScreenCenter();
    playerEntity->pos = middle;

    // Timer

    asteroidSpawn.maxValue = 1;
    asteroidSpawn.ResetCounter();

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

    if(state == GameState::Game) {
        if(asteroidSpawn.UpdateCounter(GetFrameTime())) {
            AsteroidComponent::SpawnAsteroid(ctx);
            asteroidSpawn.ResetCounter();
        }
    }
}

void GameManager::End() {
    
}

// === UI Manager ===

UIManager::UIManager(GameContext* ctx, EntityID id): IComponent(ctx, id) {}
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

    pos.y += 50;
    fontSize = 24;
    origin = MeasureTextEx(GetFontDefault(), tryAgainText.c_str(), fontSize, spacing);
    origin.y = origin.y / 2;
    origin.x = origin.x / 2;

    DrawTextPro(
        GetFontDefault(), 
        tryAgainText.c_str(), 
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

PlayerControllerComponent::PlayerControllerComponent(GameContext* ctx, EntityID id): IComponent(ctx, id) {}

void PlayerControllerComponent::ReceiveData(ColliderEvent event) {
    Die();
}

void PlayerControllerComponent::Die() {
    PublishData(PlayerEvent::PlayerDied);
    Entity::Destroy(ctx, id);
}

void PlayerControllerComponent::Start() {
    RenderComponent* rc = Entity::FindComponent<RenderComponent>(ctx, id).value();
    CircleColliderComponent* cc = Entity::FindComponent<CircleColliderComponent>(ctx, id).value();


    Texture2DResource texture = ctx->resourceManager->GetTexture(TextureNames::PlayerTexture).value();
    rc->SetCurrentTexture(texture);

    cc->RegisterSubscriber(this);
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

// === Meteor Component ===

AsteroidComponent::AsteroidComponent(GameContext* ctx, EntityID id) 
    : IComponent(ctx, id), timeToDeath(15) {}

void AsteroidComponent::SpawnAsteroid(GameContext *ctx) {
    EntityID mainGame = Entity::GetEntityByName(ctx, "MainGame").value();

    EntityID asteroid = Entity::New(ctx, mainGame);
    EntityComponent* asteroidEntity = Entity::GetEntityComponent(ctx, asteroid);

    asteroidEntity->scale = {1.2, 1.2};

    auto pcc = std::make_unique<AsteroidComponent>(ctx, asteroid);
    auto rc = std::make_unique<RenderComponent>(ctx, asteroid);
    auto cc = std::make_unique<CircleColliderComponent>(ctx, asteroid);
    cc->SetCollider(40);

    Entity::AddComponent(ctx, asteroid, std::move(pcc));
    Entity::AddComponent(ctx, asteroid, std::move(rc));
    Entity::AddComponent(ctx, asteroid, std::move(cc));
}

void AsteroidComponent::Start() {
    EntityComponent* ec = Entity::GetEntityComponent(ctx, id);

    Vector2 center = ScreenCenter();
    f32 r = GetRandomValue(50, 100);

    ec->pos = RandomPointOnCircleEdge(center, 1000);
    Vector2 randomPoint = RandomPointOnCircleEdge(center, r);

    this->moveVector = Vector2Normalize(randomPoint - ec->pos);
    this->speed = GetRandomValue(200, 300);

    RenderComponent* rc = Entity::FindComponent<RenderComponent>(ctx, id).value();
    rc->SetCurrentTexture(ctx->resourceManager->GetTexture(TextureNames::AsteroidTexture).value());

    spdlog::debug("Spawned meteor");
}

void AsteroidComponent::Update() {
    EntityComponent* ec = Entity::GetEntityComponent(ctx, id);

    ec->rotation += (speed) * GetFrameTime();
    ec->pos += this->moveVector * speed * GetFrameTime();

    if(this->timeToDeath.UpdateCounter(GetFrameTime())) {
        Entity::Destroy(ctx, this->GetID());
        spdlog::info("Destroyed asteroid");
    }
}

void AsteroidComponent::End() {}

// === Render Component ===

RenderComponent::RenderComponent(GameContext* ctx, EntityID id) : 
    IComponent(ctx, id) {}

void RenderComponent::SetCurrentTexture(Texture2DResource texture) {
    this->currentTexture = texture;
}

void RenderComponent::Update() {
    EntityComponent* entity = Entity::GetEntityComponent(ctx, id);

    Vector2 pos = entity->pos;
    Vector2 destSize = {currentTexture.GetFrameSize().x * entity->scale.x, currentTexture.GetFrameSize().y * entity->scale.y};
    Vector2 origin;
    origin.x = destSize.x/2;
    origin.y = destSize.y/2;

    DrawTexturePro(
    currentTexture.texture, 
    currentTexture.GetFrame(0), 
    {pos.x, pos.y, destSize.x,destSize.y}, 
    origin, 
    entity->rotation + 90, 
    WHITE
    );
}