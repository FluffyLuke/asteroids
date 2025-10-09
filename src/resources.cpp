#include "raylib.h"
#include "spdlog/spdlog.h"

#include "resources.hpp"
#include "utils.hpp"

void ResourceManager::Init(std::string pathToProgram) {
    this->pathToProgram = pathToProgram;
}

ResourceID ResourceManager::CreateTexture2DResource(std::string path, Vector2Int dimensions) {
    std::string fullPath = this->pathToProgram + path;
    Texture2D texture = LoadTexture(path.c_str());

    if (texture.id <= 0) {
        spdlog::error("Could not load texture: {0}", fullPath);
        return INVALID_RESOURCE;
    }

    Texture2DResource resource {
        texture,
        dimensions,
    };

    this->textures.push_back(resource);

    return counter++;
}

Texture2DResource ResourceManager::GetTexture2DResource(ResourceID id) {
    return this->textures[id];
}

