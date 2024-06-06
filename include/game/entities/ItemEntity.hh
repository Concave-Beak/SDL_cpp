#pragma once

#include <SDL2/SDL.h>

#include "../items/Item.hh"
#include "./Entity.hh"

class ItemEntity {
   public:
    ItemEntity(Item item_);
    void PickUp();

   private:
    SDL_Texture *texture = nullptr;
    Item item;
    Entity entity;

    float physicsWeight = 0;
};
