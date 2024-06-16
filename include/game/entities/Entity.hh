#pragma once

#include <SDL2/SDL_render.h>

#include <cstdlib>
#include <vector>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"
#include "../../main/Level.hh"

class Entity {
   public:
    enum EntityType {
        PLAYER = 0,
        GENERIC_HUMANOID_ENEMY = 100,
        ITEM_ENTITY = 200,
        ATTACK = 300,
        ARROW = 400,
    };

    bool GetCollidedInformation(Direction direction);

    Vec2<float> GetVelocityNow();
    Vec2<float> GetPos();

    Vec2<int> GetHitbox();

    Direction GetFacingDirection();

    virtual void Move(const Direction& direction, const Vec2<float>& accelSpeed, const bool& isPaused);

    static void Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused, const Vec2<int>& cameraPos, SDL_Renderer* renderer);

    void Init(EntityType Type);

    SDL_Rect GetEntityRect();

    void Damage(int damage);

   protected:
    Entity();
    virtual ~Entity();

   protected:
    bool collidedDown = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedUp = false;
    bool isAbovePlatform = true;

    EntityType Type;
    Uint16 ID;

    Vec2<float> velocityNow = {0, 0};
    Vec2<float> positionNow = {0, 0};

    Direction facing = Direction::RIGHT;

    Vec2<Uint8> hitbox = {64, 64};

    float surfaceAttrition = 0;

    float healthMax = 10;
    float heathNow = 10;
    bool isDead = false;

   protected:
    void ResetCollisionState();

    Entity* GetEntity();

   private:
    virtual void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
    virtual void HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);

    virtual void HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                         const float& timeDelta, const float& timeMultiplier);
    virtual void HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                           const float& timeDelta, const float& timeMultiplier);
    virtual void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer);

   protected:
    static std::vector<Entity*> entityVector;
};

//------------------------------------------------------------------------------
