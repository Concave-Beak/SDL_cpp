#pragma once

#include <array>

#include "../../../../lib/utils/math_utils.hh"
#include "../Entity.hh"

namespace Attack {
enum class AttackType {
    ARROW_PROJECTILE = 0,
    SWORD_SLASH = 1,
    MUSKET_BALL = 2,
};

struct WeaponStats {
    WeaponStats(AttackType atkType);
    ~WeaponStats() = default;
    Entity *entityOrigin = nullptr;
    bool canHitOrigin = false;
    Uint32 canHitOriginAfter = -1;  // Number of ticks after being spawned

    float damage;  // TODO: divide damage into different types, eg: piercing, bludgeoning...

    Uint32 lifeEndTick = -1;  // Number of ticks after being spawned

    int timesHit = 0;
    int maxHits = 1;

    bool isEffectedByGravity = false;
};

class Arrow : public Entity {
    WeaponStats GetWeaponStats();

   protected:
    WeaponStats weaponStats;
    Quad<float> quad;
    float angle = 0;
    Vec2<float> maximumVelocity;

    bool isStuckToSurface = false;
    bool isStuckToEntity = false;  // if is stuck and not on entity is stuck on floor
    Entity *stuckEntity = nullptr;
    Vec2<int> posStuck;  // if stuck on entity this is the pos related to the entity

   protected:
    Arrow(Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity);
    ~Arrow() = default;

   protected:
    void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) override;

    void HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    void HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    void HandleSurfaceCollision(const SDL_Rect &surfaceRect, const std::array<Vec2<int>, 4> &verticiesAttackRect);
    void HandleEntityCollision(Entity *entity, const std::array<Vec2<int>, 4> &modelVerticies);

   private:
    void HandleQuadRotation();
    void HandleStuck();

    void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override { (void)direction, (void)accelSpeed, (void)isPaused; };  // does nothing
};

//------------------------------------------------------------------------------

class Swing : public Entity {  // I have plans to make most items have a swing attack, including ranged ones
    WeaponStats weaponStats;
    float angle = 0;

    Quad<float> quad;

   protected:
    Swing(Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions);
    ~Swing() = default;

   protected:
    void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) override;

    void HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    void HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

   private:
    void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override;

    void HandleEntityCollision(Entity *entity);

    void HandleLifetime();
};

}  // namespace Attack
