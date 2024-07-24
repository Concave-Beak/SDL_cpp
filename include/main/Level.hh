#pragma once

#include <SDL2/SDL.h>

#include <vector>

#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

enum CollisionType {
    NO_COLLISION = 0,
    FULL_COLLISION,
    PLATFORM,
};

enum TextureID {
    VOID = -1,
    DIRT = 1,
    MUD = 2,
    WOOD = 3,
    STONE = 4,
};

struct LevelItem {
    Vec2<int> pos;
    CollisionType collisionType;
    SDL_Color color;
    SDL_Rect rect;

    TextureID textureID;   // this will be used to find the texture later on
    SDL_Texture *texture;  // there should also be a footstep sound depending on the surface
    float attritionCoefficient;

    LevelItem(Vec2<int> ppos, Vec2<int> size, CollisionType pcolType, SDL_Color pcolor,
              TextureID ptextID);
    LevelItem();
    ~LevelItem();
};

//------------------------------------------------------------------------------

class Level {
   public:
    static std::vector<LevelItem> collisions;
    static std::vector<LevelItem> textures;

    Level() {};
    ~Level() {};

    void GenerateLevel(const Uint8 &levelID);
    static void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer);

   private:
};

//------------------------------------------------------------------------------
