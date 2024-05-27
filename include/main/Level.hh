#pragma once

#include <SDL2/SDL.h>

#include <vector>

#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

enum ColisionType {
    NO_COLISION = 0,
    FULL_COLISION,
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
    Vector2<int> pos;
    ColisionType colisionType;
    SDL_Color color;
    SDL_Rect wireframe;

    TextureID textureID;   // this will be used to find the texture later on
    SDL_Texture *texture;  // there should also be a footstep sound depending on the surface
    float attritionCoefficient;

    LevelItem(Vector2<int> ppos, Vector2<int> size, ColisionType pcolType, SDL_Color pcolor,
              TextureID ptextID);
    LevelItem();
    ~LevelItem();
};

//------------------------------------------------------------------------------

class Level {
   public:
    static std::vector<LevelItem> colisions;
    static std::vector<LevelItem> textures;

    Level() {};
    ~Level() {};

    void GenerateLevel(const Uint8 &levelID);
    static void Draw(const Vector2<int> &cameraPos, SDL_Renderer *renderer);

   private:
};

//------------------------------------------------------------------------------
