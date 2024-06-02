#pragma once

#include <SDL2/SDL_render.h>

#include <vector>

#include "../../lib/utils/engine_utils.hh"
#include "../../lib/utils/math_utils.hh"

class Entity {
   public:
    enum EntityID {
        PLAYER = 0,
        GENERIC_HUMANOID_ENEMY = 100,
    };
    Entity(EntityID ID_, Vector2<float> spawnPosition, Vector2<int> hitbox_);
    ~Entity();

    bool GetColidedInformation(Direction direction);

    Vector2<float> GetVelocity();
    Vector2<float> GetAccelspeed();
    Vector2<float> GetPos();

    Vector2<int> GetHitbox();

    Direction GetFacingDirection();

    void Move(const Direction& direction, const Vector2<float>& accelSpeed, const bool& isPaused);

    static void Draw(const Vector2<int>& cameraPos, SDL_Renderer* renderer);
    static void Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);

   private:
    bool colidedDown = false;
    bool colidedLeft = false;
    bool colidedRight = false;
    bool colidedUp = false;

    bool isAbovePlatform = true;

    EntityID ID;

    Vector2<float> velocity = {0, 0};
    Vector2<float> position = {0, 0};

    Direction facing = RIGHT;

    Vector2<int> hitbox = {64, 64};

    // const SDL_Texture* StandingTexture;  // not used
    // const SDL_Texture* DuckingTexture;

    float surfaceAttrition = 0;

   private:
    static std::vector<Entity*> entityVector;

    static void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
    static void HandleColisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
};
