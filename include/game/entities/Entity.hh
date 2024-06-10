#pragma once

#include <SDL2/SDL_render.h>

#include <vector>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"
#include "../../main/Level.hh"

class Entity {
   public:
    enum EntityID {
        PLAYER = 0,
        GENERIC_HUMANOID_ENEMY = 100,
        ITEM_ENTITY = 200,
    };
    Entity(EntityID ID_, Vec2<float> spawnPosition, Vec2<int> hitbox_);
    ~Entity();

    bool GetCollidedInformation(Direction direction);

    Vec2<float> GetVelocityNow();
    Vec2<float> GetPos();

    Vec2<int> GetHitbox();

    Direction GetFacingDirection();

    void Move(const Direction& direction, const Vec2<float>& accelSpeed, const bool& isPaused);

    static void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer);
    static void Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);

   private:
    bool collidedDown = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedUp = false;

    bool isAbovePlatform = true;

    EntityID ID;

    Vec2<float> velocityNow = {0, 0};
    Vec2<float> positionNow = {0, 0};

    Direction facing = RIGHT;

    Vec2<Uint8> hitbox = {64, 64};

    // const SDL_Texture* StandingTexture;  // not used
    // const SDL_Texture* DuckingTexture;

    float surfaceAttrition = 0;

   private:
    void ResetCollisionState();

    static SDL_Rect GetEntityRect(const Entity& entity);

    static std::vector<Entity*> entityVector;

    static void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);

    static void HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
    static void HandleVerticalCollision(Entity* entity, const SDL_Rect& entityRect, const LevelItem& levelItem, const float& timeDelta, const float& timeMultiplier);
    static void HandleHorizontalCollision(Entity* entity, const SDL_Rect& entityRect, const LevelItem& levelItem, const float& timeDelta, const float& timeMultiplier);
};

//------------------------------------------------------------------------------

class Attack {
   public:
    enum AttackType {
        SWORD_SLASH = 0,
        ARROW,
    };
    Attack(Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, Uint32 lifeTime);
    ~Attack();

    static void CheckAndDestroyExpiredAttacks();
    static void Draw(const Vec2<float>& cameraPos, SDL_Renderer* renderer);

   private:
    float damage;

    Vec2<float> spawnPos = {0, 0};
    Vec2<Uint8> hitbox = {30, 20};

    Vec2<float> speedNow = {0, 0};
    // Vec2<float> accelSpeed; // Not used

    AttackType atkType;

    Uint32 lifeEndTick;

    inline static std::vector<Attack*> attackVector = {};
};
