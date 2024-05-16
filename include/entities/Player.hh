#pragma once

#include <SDL2/SDL.h>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

//------------------------------------------------------------------------------

class Player {
   public:
    Vector2<float> velocity = {0, 0};
    Vector2<float> pos = {0, 0};

    bool colidedDown = false;
    bool colidedLeft = false;
    bool colidedRight = false;
    bool colidedUp = false;

    bool isAbovePlatform = false;

    // Dash
    bool isPreparingToDash = false;
    bool isDashing = false;
    float DashEnd;
    float whenNextDashAvailable;

    float preDashAnimationEnd;
    float preDashDurationInMs;

    float angleDash = 0;  // in degrees
    //--

    Vector2<int> hitbox = {75, 75};

   public:
    static Player* GetPlayerInstace();

    // Vector2<float>* GetPlayerPos();  // not used
    void HandleVelocity(const float& delta, const float& timeMultiplier);
    void HandleColisions(const float& delta, const float& timeMultiplier);

    void Move(const MoveOptions);
    void PrepareToDash(MoveOptions moveOpt, float startTick,
                       SDL_Renderer* renderer, float* timeMultiplier);
    void Dash();

    Vector2<int> GetHitboxInfo();

   private:
    static Player* playerInstance;

    const Vector2<float> accelSpeed = {
        1,
        250,
    };  // may change during gameplay, I'll just
        // use it as a const it for now

    float attrition = 0;

    // const SDL_Texture* StandingTexture;  // not used
    // const SDL_Texture* DuckingTexture;
    MoveOptions facing = RIGHT;
};

//------------------------------------------------------------------------------
