#pragma once

#include <SDL2/SDL.h>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

class Player {
   public:
    bool colidedDown = false;
    bool colidedLeft = false;
    bool colidedRight = false;
    bool colidedUp = false;

    bool isAbovePlatform = false;

    Vector2<int> hitbox = {64, 64};

   public:
    static Player* GetPlayerInstace();

    void Handle(const float& delta, const float& timeMultiplier, const bool& isPaused);

    Vector2<float> GetPos();
    Vector2<float> GetVelocity();
    void Draw(const Vector2<int>& cameraPos, SDL_Renderer* renderer);

    void Move(const Directions&, const bool& isPaused);

    Vector2<int> GetHitboxInfo();

   private:
    static Player* playerInstance;

    Vector2<float> velocity = {0, 0};
    Vector2<float> pos = {0, 0};
    const Vector2<float> accelSpeed = {1, 200};  // may change during gameplay, I'll just
                                                 // use it as a const it for now

    float attrition = 0;

    // const SDL_Texture* StandingTexture;  // not used
    // const SDL_Texture* DuckingTexture;
    Directions facing = RIGHT;

    void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
    void HandleColisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
};

//------------------------------------------------------------------------------
