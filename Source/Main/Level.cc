#include "./Level.hh"
#include <SDL2/SDL_render.h>
#include <vector>
#include "../../Include/Headers/Utils.hh"

std::vector<LevelItem> Level::colisions;
std::vector<LevelItem> Level::textures = {};

//------------------------------------------------------------------------------

LevelItem::~LevelItem(){};
LevelItem::LevelItem(){};
LevelItem::LevelItem(Vec2i ppos, Vec2i size, ColisionType pcolType, SDL_Color pcolor) : pos(ppos), colisionType(pcolType), color(pcolor) {
    this->wireframe = {ppos.x, ppos.y, size.x, size.y};

    if (pcolType != NO_COLISION) {
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
        SDL_Color color = levelItem.color;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &levelItem.wireframe);
    }

    return true;
}
