#ifndef __MAP_HEADER__
#define __MAP_HEADER__

#include <SDL2/SDL.h>

#include <vector>

#include "../../Include/Headers/Utils.hh"

//------------------------------------------------------------------------------

enum ColisionType {
    NO_COLISION = 0,
    FULL_COLISION,
    PLATFORM,
};

struct LevelItem {
    // std::unordered_map<std::string, float> TerrainType{
    //     {"dirt", 1.0},
    //     {"mud", 0.8},
    // };
    // Not implemented

    Vec2i pos;
    ColisionType colisionType;
    SDL_Color color;
    SDL_Rect wireframe;
    // SDL_Texture *texture; // not used

    LevelItem(Vec2i ppos, Vec2i size, ColisionType pcolType, SDL_Color pcolor);
    LevelItem();
    ~LevelItem();

   private:
};

//------------------------------------------------------------------------------

class Level {
   public:
    static std::vector<LevelItem> colisions;
    static std::vector<LevelItem> textures;
};

//------------------------------------------------------------------------------

#endif
