#include "../../include/main/Level.hh"

#include <SDL2/SDL_render.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "../../lib/utils/math_utils.hh"

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
LevelItem::LevelItem(Vec2i ppos, Vec2i size, ColisionType pcolType, SDL_Color pcolor, TextureID ptextID) : pos(ppos), colisionType(pcolType), color(pcolor), textureID(ptextID) {
    this->wireframe = {ppos.x, ppos.y, size.x, size.y};
    switch (ptextID) {
        case VOID: {  // this is where the textures should be applied
            this->attritionCoefficient = 0;
            break;
        }
        case DIRT: {
            this->attritionCoefficient = 2;
            break;
        }
        case MUD: {
            this->attritionCoefficient = 3.5;
            break;
        }
        case WOOD: {
            this->attritionCoefficient = 2;
            break;
        }
        case STONE: {
            this->attritionCoefficient = 2;
            break;
        }
    }

    if (pcolType != NO_COLISION) {
        Level::colisions.push_back(*this);  // This is used when checking
                                            // for colisions, because I dont
                                            // want to iterate through every LevelItem
    } else {
        Level::textures.push_back(*this);
    }
}

//------------------------------------------------------------------------------

void Level::GenerateLevel(const Uint8 &levelID) {
}

//------------------------------------------------------------------------------
