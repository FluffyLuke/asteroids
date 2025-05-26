#ifndef __GAME__
#define __GAME__

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "raylib.h"

namespace texture_names {
    const std::string PlayerTexture = "PlayerTexture";
}

// === Utils ===

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

class GameObject {
    private:

    const GameContext* ctx;

    public:

    std::vector<std::unique_ptr<IComponent>> components;
    Vector2 pos;
    float scale = 1;
    float rotation = 0;

    GameObject(GameContext* ctx);
    void AddComponent(std::unique_ptr<IComponent> component);
    template<class TComponent>
    void RemoveComponent();
};

class GameContext {
    public:

    GameContext();
    ~GameContext(); // declaration only

    std::unique_ptr<ResourceManager> resourceManager;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    // std::shared_ptr<GameObject> NewGameObject() {
    GameObject* NewGameObject();
};

// === Components ===

class IComponent {
    protected:

    GameContext* ctx;

    public:

    GameObject* gameObject;

    IComponent(GameContext* ctx, GameObject* gameObject);

    // Life time function
    virtual void Start() {};
    virtual void Update() {};
    virtual void End() {};

    virtual ~IComponent() {};
};

class PlayerControllerComponent : public IComponent {
    public:

    PlayerControllerComponent(GameContext* ctx, GameObject* gameObject);
    void Update();
};

class RenderComponent : public IComponent {
    public:
    
    RenderComponent(GameContext* ctx, GameObject* gameObject);
    void Update();
};

enum GameState {
    MainMenu,
    Game,
    GameOver,
};

class GameManager : public IComponent {
    private:

    GameState state;
    PlayerControllerComponent* spawnPlayer();
    static GameManager* instance;

    public:
    GameManager(const GameManager& obj) = delete;
    GameManager(GameContext* ctx, GameObject* gameObject);

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

    UIManager(GameContext* ctx, GameObject* gameObject);
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