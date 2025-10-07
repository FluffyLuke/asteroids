#ifndef __GAME__
#define __GAME__

#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "raylib.h"

#include "spdlog/spdlog.h"
#include "utils.hpp"

namespace TextureNames {
    const std::string PlayerTexture = "PlayerTexture";
    const std::string AsteroidTexture = "AsteroidTexture";
}

// Forward declarations

class IComponent;
class GameContext;
class ResourceManager;
class EntityComponent;

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

    void RegisterSubscriber(Subscriber<T>* subscriber) {
        subscribers.push_back(subscriber);
    }

    void PublishData(T data) {
        for(auto &subscriber : subscribers) {
            subscriber->ReceiveData(data);
        }
    }
};

// === Resources ===

struct Texture2DResource {
    Texture2D texture;
    Vector2 size;
    Vector2 frames;

    Texture2DResource();
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

// === Main stuff === 

using EntityID = i64;

class GameContext {
    public:

    GameContext();
    ~GameContext();

    std::unique_ptr<ResourceManager> resourceManager;
    std::unordered_map<EntityID, std::list<std::unique_ptr<IComponent>>> entities;
    std::vector<EntityID> topEntities;

    std::vector<EntityID> newEntities;
    std::vector<EntityID> deadEntities;
};

class Entity {
    static i64 index;

    public:

    static void AddComponent(GameContext* ctx, EntityID id, std::unique_ptr<IComponent> component);
    static EntityComponent* GetEntityComponent(GameContext* ctx, EntityID id);
    template<typename T>
    static std::optional<T*> FindComponent(GameContext* ctx, EntityID id) {
        try {
            auto& components = ctx->entities.at(id);
            for(auto&& c : components) {
                if(auto ptr = dynamic_cast<T*>(c.get())) {
                    return std::optional<T*>{ptr};
                }
            }
        } catch(const std::out_of_range& e) {
            spdlog::error("Entity ID {} not found: {}", id, e.what());
        }

        return std::nullopt;
    }
    template<typename T>
    static std::vector<T*> FindComponents(GameContext* ctx) {
        std::vector<T*> components;

        for(auto& e : ctx->entities) {
            for(auto&& c : e.second) {
                if(auto ptr = dynamic_cast<T*>(c.get())) {
                    components.push_back(ptr);
                }
            }
        }

        return components;
    }
    template<typename T>
    static std::vector<T*> FindOtherComponents(GameContext* ctx, EntityID id) {
        std::vector<T*> components;

        for(auto& e : ctx->entities) {
            for(auto&& c : e.second) {
                if(id == e.first) continue;
                if(auto ptr = dynamic_cast<T*>(c.get())) {
                    components.push_back(ptr);
                }
            }
        }

        return components;
    }
    static std::optional<EntityID> GetEntityByName(GameContext* ctx, std::string name);

    static void SetParent(GameContext* ctx, EntityID child_id, EntityID newParent_id);
    static bool RemoveParent(GameContext* ctx, EntityID child_id);

    static EntityID New(GameContext* ctx);
    static EntityID New(GameContext* ctx, EntityID parent_id);
    static EntityID New(GameContext* ctx, EntityComponent* parent);

    static EntityID New(GameContext* ctx, std::string name);
    static EntityID New(GameContext* ctx, EntityID parent_id, std::string name);
    static EntityID New(GameContext* ctx, EntityComponent* parent, std::string name);

    static void Destroy(GameContext* ctx, EntityID id);
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
    Vector2 pos = {0, 0};
    Vector2 scale = {1, 1};
    float rotation = 0;

    std::optional<EntityID> parent;
    std::vector<EntityID> children;

    EntityComponent(GameContext* ctx, EntityID id);

    void Start();
};

struct ColliderEvent {
    EntityID collidedWith;
};

class CircleColliderComponent : public IComponent, public Publisher<ColliderEvent> {
    public:

    f32 radius;

    void SetCollider(f32 radius);

    CircleColliderComponent(GameContext* ctx, EntityID id);
    void Update();
};


enum PlayerEvent {
    PlayerDied
};

class PlayerControllerComponent : 
    public IComponent, 
    public Publisher<PlayerEvent>, 
    public Subscriber<ColliderEvent> 
{
    public:

    void Die();
    void ReceiveData(ColliderEvent event);

    PlayerControllerComponent(GameContext* ctx, EntityID id);
    void Start();
    void Update();
};

class RenderComponent : public IComponent {
    private:

    Texture2DResource currentTexture;

    public:
    
    void SetCurrentTexture(Texture2DResource texture);

    RenderComponent(GameContext* ctx, EntityID id);
    void Update();
};

enum GameState {
    MainMenu,
    Game,
    GameOver,
};

class GameManager : public IComponent, public Subscriber<PlayerEvent> {
    private:

    static GameManager* instance;

    GameState state;
    Counter asteroidSpawn;

    PlayerControllerComponent* spawnPlayer();

    public:

    float timeAlive;

    GameManager(const GameManager& obj) = delete;
    GameManager(GameContext* ctx, EntityID id);
    static GameManager* getInstance();
    
    static void CreateGameManager(GameContext* ctx);
    void ReceiveData(PlayerEvent event);

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

class AsteroidComponent : public IComponent {
    private:

    f32 speed = 0;
    Vector2 moveVector = {0, 0};

    Counter timeToDeath;

    public:

    static void SpawnAsteroid(GameContext* ctx);

    AsteroidComponent(GameContext* ctx, EntityID id);
    void Start();
    void Update();
    void End();
};

#endif