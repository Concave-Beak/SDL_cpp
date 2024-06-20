#pragma once

#include <SDL2/SDL.h>

#include "./Entity.hh"

class ItemEntity : protected Entity {
   public:
    ItemEntity();
    void PickUp();

   private:
    SDL_Texture *texture = nullptr;
    // Item item;

    float physicsWeight = 0;
};
