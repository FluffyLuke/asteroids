#ifndef __RESOURCES__
#define __RESOURCES__

#include <cstdint>
#include <string>
#include <vector>

#include "raylib.h"

#include "utils.hpp"

using ResourceID = u64;
constexpr ResourceID INVALID_RESOURCE = UINT64_MAX;

struct Texture2DResource {
    Texture2D texture;
    Vector2Int size;
};

class ResourceManager {
    private:
    ResourceID counter = 0;
    std::string pathToProgram;

    std::vector<Texture2DResource> textures;

    public:
    void Init(std::string pathToProgram);

    ResourceID CreateTexture2DResource(std::string path, Vector2Int dimensions);
    Texture2DResource GetTexture2DResource(ResourceID id);
};


#endif