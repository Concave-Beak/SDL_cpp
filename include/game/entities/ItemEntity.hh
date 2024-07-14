#pragma once

#include <SDL2/SDL.h>

class ItemEntity {
   public:
    ItemEntity();
    void PickUp();

   private:
    SDL_Texture *texture = nullptr;
    // Item item;

    float physicsWeight = 0;
};
