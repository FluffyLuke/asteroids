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
    virtual void RecvPublishedData(T);
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

    void PublishData(std::string data) {
        for(auto &subscriber : subscribers) {
            subscriber->RecvPublishedData(data);
        }
    }
};

// === Main stuff === 

class IComponent;
class GameContext;
class ResourceManager;

using EntityID = i64;

class GameObject {
    static i64 index;

    public:

    template<typename T>
    void AddComponent(GameContext* ctx, EntityID id, T component);
    template<typename T>
    static std::optional<T*> FindComponent(GameContext* ctx, EntityID id);    

    EntityID New(GameContext* ctx);
    void Destroy(GameContext* ctx, EntityID id);
};

class GameContext {
    public:

    GameContext();
    ~GameContext();

    std::unique_ptr<ResourceManager> resourceManager;
    std::unordered_map<EntityID, std::list<std::unique_ptr<IComponent>>> entities;
    std::vector<EntityID> deadEntities;
};

// === Components ===

class IComponent {
    protected:

    GameContext* ctx;
    EntityID id;

    public:

    IComponent(GameContext* ctx, EntityID gameObject);

    // Life time function
    virtual void Start() {};
    virtual void Update() {};
    virtual void LateUpdate() {};
    virtual void End() {};

    virtual ~IComponent() {};
};

class PlayerControllerComponent : public IComponent {
    public:

    PlayerControllerComponent(GameContext* ctx, EntityID gameObject);

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

class GameManager : public IComponent {
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
    void LateUpdate();
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