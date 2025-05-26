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

bool InsideRec()

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

class GameManager : public IComponent {
    private:
    PlayerControllerComponent* spawnPlayer();

    public:
    
    static void CreateGameManager(GameContext* ctx);

    GameManager(GameContext* ctx, GameObject* gameObject);
    void Start();
    void Update();
    void End();
};

class UIManager : public IComponent {
    public:
    
    static void CreateUIManager(GameContext* ctx);

    UIManager(GameContext* ctx, GameObject* gameObject);
    void Start();
    void Update();
    void End();

    void ShowStartScreen();
    void ShowGameScreen();
    void ShowEndScreen();
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