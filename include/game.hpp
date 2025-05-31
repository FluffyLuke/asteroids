#ifndef __GAME__
#define __GAME__

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "raylib.h"

namespace texture_names {
    const std::string PlayerTexture = "PlayerTexture";
}

// === Utils ===

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

Vector2 ScreenCenter();

bool InsideRec(Rectangle rec, Vector2 point);

// https://gist.github.com/bosley/6e844b7d0f53e807fed0fd8c33828e80
template<typename T>
class Subscriber {
    public:

    virtual ~Subscriber() = default;
    virtual void ReceiveData(T) = 0;
};

template<typename T>
class Publisher {
    private:

    std::vector<Subscriber<T>*> subscribers;

    public:

    virtual ~Publisher() = default;
    int GetSubscriberCount() const {

        return subscribers.size();
    }

    void RegisterSubscriber(Subscriber<T> & subscriber) {
        subscribers.push_back(&subscriber);
    }

    void PublishData(T data) {
        for(auto &subscriber : subscribers) {
            subscriber->ReceiveData(data);
        }
    }
};

// === Main stuff === 

class IComponent;
class GameContext;
class ResourceManager;
class EntityComponent;

using EntityID = i64;

class Entity {
    static i64 index;

    public:

    static void AddComponent(GameContext* ctx, EntityID id, std::unique_ptr<IComponent> component);
    static EntityComponent* GetEntityComponent(GameContext* ctx, EntityID id);
    template<typename T>
    static std::optional<T*> FindComponent(GameContext* ctx, EntityID id);
    
    static std::optional<EntityID> GetEntityByName(GameContext* ctx, std::string name);

    static EntityID New(GameContext* ctx);
    static EntityID New(GameContext* ctx, EntityID parent_id);
    static EntityID New(GameContext* ctx, EntityComponent* parent);

    static EntityID New(GameContext* ctx, std::string name);
    static EntityID New(GameContext* ctx, EntityID parent_id, std::string name);
    static EntityID New(GameContext* ctx, EntityComponent* parent, std::string name);

    static void Destroy(GameContext* ctx, EntityID id);
};

class GameContext {
    public:

    GameContext();
    ~GameContext();

    std::unique_ptr<ResourceManager> resourceManager;
    std::unordered_map<EntityID, std::list<std::unique_ptr<IComponent>>> entities;
    std::vector<EntityID> topEntities;

    std::vector<EntityID> deadEntities;
};

// === Components ===

class IComponent {
    protected:

    GameContext* ctx;
    EntityID id;

    public:

    IComponent(GameContext* ctx, EntityID id);

    EntityID GetID();

    virtual void Start() {};
    virtual void Update() {};
    virtual void End() {};

    virtual ~IComponent() {};
};

class EntityComponent : public IComponent {
    public:

    std::string name = "Empty";
    Vector2 pos = {0,0};
    Vector2 scale = {1, 1};
    float rotation = 0;

    std::optional<EntityID> parent;
    std::vector<EntityID> children;

    EntityComponent(GameContext* ctx, EntityID id);

    void Start();
};

namespace PlayerEvent {
    enum PlayerEvent {
        PlayerDied
    };
}

class PlayerControllerComponent : public IComponent, public Publisher<PlayerEvent::PlayerEvent> {
    public:

    void Die();

    PlayerControllerComponent(GameContext* ctx, EntityID id);

    void Update();
};

class RenderComponent : public IComponent {
    public:
    
    RenderComponent(GameContext* ctx, EntityID id);
    void Update();
};

enum GameState {
    MainMenu,
    Game,
    GameOver,
};

class GameManager : public IComponent, public Subscriber<PlayerEvent::PlayerEvent> {
    private:

    static GameManager* instance;

    GameState state;

    PlayerControllerComponent* spawnPlayer();

    public:

    float timeAlive;

    GameManager(const GameManager& obj) = delete;
    GameManager(GameContext* ctx, EntityID id);
    static GameManager* getInstance();
    
    static void CreateGameManager(GameContext* ctx);
    void ReceiveData(PlayerEvent::PlayerEvent event);

    void StartGame();
    GameState GetGameState();

    void Start();
    void Update();
    void End();
};

class UIManager : public IComponent {
    private:

    void renderStartScreen();
    void renderGameScreen();
    void renderEndScreen();

    public:
    
    static void CreateUIManager(GameContext* ctx);

    UIManager(GameContext* ctx, EntityID id);
    void Start();
    void Update();
    void End();

    void RecvPublishedData(GameState);
};

// === Resources ===

struct Texture2DResource {
    Texture2D texture;
    Vector2 size;
    Vector2 frames;

    Texture2DResource(Texture2D texture, Vector2 size, Vector2 frames);

    Vector2 GetFrameSize();

    Rectangle GetFrame(int frameIndex);
};

class ResourceManager {
    private:
    
    std::unordered_map<std::string, Texture2DResource> textures;

    public:

    void LoadTexture(std::string textureName, Vector2 size, Vector2 frames, std::string pathToTexture);
    std::optional<Texture2DResource> GetTexture(std::string textureName);
    
    void FreeResources();
};

#endif