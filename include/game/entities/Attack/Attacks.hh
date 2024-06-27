#pragma once

#include <array>

#include "../../../../lib/utils/math_utils.hh"
#include "../Entity.hh"

namespace Attacks {
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

class ArrowProjectile : public Entity {
    WeaponStats GetWeaponStats();

   protected:
    WeaponStats weaponStats;
    float angle = 0;
    Vec2<float> maximumVelocity;

    bool isStuckToSurface = false;
    bool isStuckToEntity = false;  // if is stuck and not on entity is stuck on floor
    Entity *stuckEntity = nullptr;
    Vec2<int> posStuck;  // if stuck on entity this is the pos related to the entity

    Quad<float> quad;

   protected:
    ArrowProjectile(float angle, Vec2<float> posNow_, Vec2<float> dimentions, Vec2<float> velocity);
    ~ArrowProjectile() = default;

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
}  // namespace Attacks
