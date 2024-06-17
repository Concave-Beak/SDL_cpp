#include "../../include/main/Level.hh"

#include <SDL2/SDL_render.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "../../lib/utils/math_utils.hh"
#include "../../lib/utils/sdl_utils.hh"

std::vector<LevelItem> Level::collisions;
std::vector<LevelItem> Level::textures = {};

//------------------------------------------------------------------------------

LevelItem::~LevelItem() {  // TODO
    // if (this->colisionType == NO_COLISION) {
    //     typedef std::vector<LevelItem>::iterator iterator;  // Define the iterator type
    //
    //     for (iterator it = Level::colisions.begin(); it != Level::colisions.end(); ++it) {
    //         if (&(*it) == item) {
    //             Level::colisions.erase(it);
    //             break;
    //         }
    //     }
    // } else {
    //     for (size_t i = 0; i < Level::textures.size(); i++) {
    //         LevelItem *colItem = &Level::textures[i];
    //         if (colItem == this) {
    //             Level::textures.erase(Level::textures.begin() + i);
    //             break;
    //         }
    //     }
    // }
};

LevelItem::LevelItem() {};
LevelItem::LevelItem(Vec2<int> ppos, Vec2<int> size, CollisionType pcolType, SDL_Color pcolor, TextureID ptextID) : pos(ppos), collisionType(pcolType), color(pcolor), textureID(ptextID) {
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

    if (pcolType != NO_COLLISION) {
        Level::collisions.push_back(*this);  // This is used when checking
                                             // for colisions, because I dont
                                             // want to iterate through every LevelItem
    } else {
        Level::textures.push_back(*this);
    }
}

//------------------------------------------------------------------------------

void Level::GenerateLevel(const Uint8 &levelID) { (void)levelID; }

void Level::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    for (LevelItem levelItem : Level::collisions) {
        SDL_Color color = levelItem.color;
        SDL_Rect levelItemWireframe = {
            levelItem.wireframe.x - (int)cameraPos.x,
            levelItem.wireframe.y - (int)cameraPos.y,
            levelItem.wireframe.w,
            levelItem.wireframe.h,
        };
        scc(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a)).Handle();
        scc(SDL_RenderFillRect(renderer, &levelItemWireframe)).Handle();
    }
}

//------------------------------------------------------------------------------
