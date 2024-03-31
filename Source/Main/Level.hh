#ifndef __MAP_HEADER__
#define __MAP_HEADER__

#include <SDL2/SDL.h>

#include <vector>

#include "../Utils/Utils.hh"

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
    Vec2i pos;
    ColisionType colisionType;
    SDL_Color color;
    SDL_Rect wireframe;

    TextureID textureID;   // this will be used to find the texture later on
    SDL_Texture *texture;  // there should also be a footstep sound depending on the surface
    float attritionCoefficient;

    LevelItem(Vec2i ppos, Vec2i size, ColisionType pcolType, SDL_Color pcolor, TextureID ptextID);
    LevelItem();
    ~LevelItem();
};

//------------------------------------------------------------------------------

class Level {
   public:
    static std::vector<LevelItem> colisions;
    static std::vector<LevelItem> textures;
};

//------------------------------------------------------------------------------

#endif
