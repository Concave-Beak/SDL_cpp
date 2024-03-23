#include "./Level.hh"

#include <SDL2/SDL_render.h>

#include <vector>

#include "../../Include/Headers/Utils.hh"

std::vector<LevelItem> Level::colisions;
std::vector<LevelItem> Level::textures = {};

//------------------------------------------------------------------------------

LevelItem::~LevelItem() {
    if (this->colisionType == NO_COLISION) {
        for (size_t i = 0; i < Level::colisions.size(); i++) {
            LevelItem *colItem = &Level::colisions[i];
            if (colItem == this) {
                Level::colisions.erase(Level::colisions.begin() + i);
                break;
            }
        }
    } else {
        for (size_t i = 0; i < Level::textures.size(); i++) {
            LevelItem *colItem = &Level::textures[i];
            if (colItem == this) {
                Level::textures.erase(Level::textures.begin() + i);
                break;
            }
        }
    }
};

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
}

//------------------------------------------------------------------------------
