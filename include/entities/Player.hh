#pragma once

#include <SDL2/SDL.h>

#include "../../lib/utils/math_utils.hh"
#include "./Entity.hh"

//------------------------------------------------------------------------------

class Player {
   public:
    static Player* GetPlayerInstace();

    Vector2<float> GetPos();
    Vector2<float> GetVelocity();

    Vector2<int> GetHitbox();

    Entity entity = Entity(Entity::PLAYER, {1, 200}, {0, 0}, {64, 64});

   private:
    static Player* playerInstance;
};

//------------------------------------------------------------------------------
