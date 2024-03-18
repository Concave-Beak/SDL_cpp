#ifndef __MAP_HEADER__
#define __MAP_HEADER__

#include <SDL2/SDL.h>
#include <vector>
#include "../../Include/Headers/Utils.hh"

//------------------------------------------------------------------------------

enum ColisionType {
    NO_COLISION = 0,
    FULL_COLISION,
    FULL_COLISION_AND_TEXTURE,
};

struct LevelItem {
    Vec2i pos;
    ColisionType colisionType;
    SDL_Color color;
    SDL_Rect wireframe;
    // SDL_Texture *texture; // not used

    LevelItem(Vec2i ppos, ColisionType pcolType, SDL_Color pcolor);
    LevelItem();
    ~LevelItem();

   private:
};

//------------------------------------------------------------------------------

class Level {
   public:
    static std::vector<LevelItem> colisions;
    static std::vector<LevelItem> textures;
    static bool GenerateLevel(int level, SDL_Renderer *renderer);
};

//------------------------------------------------------------------------------

#endif
