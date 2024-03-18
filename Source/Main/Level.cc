#include "./Level.hh"
#include <SDL2/SDL_render.h>
#include <vector>
#include "../../Include/Headers/Utils.hh"

std::vector<LevelItem> Level::colisions;
std::vector<LevelItem> Level::textures = {};

//------------------------------------------------------------------------------

LevelItem::~LevelItem(){};
LevelItem::LevelItem(){};
LevelItem::LevelItem(Vec2i ppos, ColisionType pcolType, SDL_Color pcolor) : pos(ppos), colisionType(pcolType), color(pcolor) {
    this->wireframe = {ppos.x, ppos.y, 75, 75};

    if (pcolType == FULL_COLISION) {
        Level::colisions.push_back(*this);  // This is used when checking
                                            // for colisions, because I dont
                                            // want to iterate through every LevelItem
    } else {
        Level::textures.push_back(*this);
    }
};

//------------------------------------------------------------------------------

bool Level::GenerateLevel(int level, SDL_Renderer *renderer) {
    (void)level;
    for (LevelItem levelItem : colisions) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
        SDL_RenderFillRect(renderer, &levelItem.wireframe);
    }

    return true;
}
