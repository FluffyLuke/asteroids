#include "game.hpp"
#include "raylib.h"
#include "spdlog/spdlog.h"
#include <optional>

Texture2DResource::Texture2DResource(Texture2D texture, Vector2 size, Vector2 frames) {
    this->texture = texture;
    this->size = size;
    this->frames = frames;
}
Vector2 Texture2DResource::GetFrameSize() {
    return {
        size.x / frames.x,
        size.y / frames.y,
    };
}
Rectangle Texture2DResource::GetFrame(int frameIndex) {
    int frameX = frameIndex != 0 ? (int)(frames.x) % frameIndex : 0;
    int frameY = frameIndex != 0 ? (int)(frames.y) / frameIndex : 0;

    Vector2 frameSize = GetFrameSize();
    float x = frameSize.x * frameX;
    float y = frameSize.y * frameY;

    return Rectangle {
        x,
        y,
        frameSize.x,
        frameSize.y
    };
}

void ResourceManager::LoadTexture(std::string textureName, Vector2 size, Vector2 frames, std::string pathToTexture) {
    Image image = LoadImage(pathToTexture.c_str());
    Texture2D texture = LoadTextureFromImage(image);
    //UnloadImage(image);

    textures.insert({textureName, Texture2DResource(texture, size, frames)});
}
std::optional<Texture2DResource> ResourceManager::GetTexture(std::string textureName) {
    if(textures.contains(textureName)) {
        return std::make_optional(textures.at(textureName));
    }
    spdlog::error("Cannot find texture!");
    return std::nullopt;
}
void ResourceManager::FreeResources() {
    for(const auto& x : textures) {
        Texture2DResource t = x.second;
        UnloadTexture(t.texture);
    }
    textures.clear();
}