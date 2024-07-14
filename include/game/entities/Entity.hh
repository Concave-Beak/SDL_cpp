#pragma once

#include <SDL2/SDL_render.h>

#include <cstdlib>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"

struct CombatAttributes {
    Uint32 itemCooldown = 0;

    float healthMax = 10;
    float heathNow = 10;

    bool isDead = false;
};

struct EntityAttributes {
    EntityAttributes();

    Vec2<float> walkingSpeed = {0, 0};
    Vec2<float> runningSpeed = {0, 0};

    Vec2<float> velocityNow = {0, 0};

    Vec2<float> positionNow = {0, 0};
    Vec2<int> spawnPos = {0, 0};

    Direction facing = Direction::RIGHT;

    SDL_Rect model = {.x = (int)positionNow.x, .y = (int)positionNow.y, .w = 0, .h = 0};

    inline Uint32 GetID();

   private:
    Uint32 id;

    inline static Uint32 maxUsedID = 0;
};

struct CollisionAttributes {
    bool collidedDown = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedUp = false;
    bool isAbovePlatform = true;

    float surfaceAttrition = 0;
};

//------------------------------------------------------------------------------
