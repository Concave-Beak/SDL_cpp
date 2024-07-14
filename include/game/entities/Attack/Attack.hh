#pragma once

#include <array>
#include <memory>
#include <vector>

#include "../../../../lib/utils/math_utils.hh"
#include "../../items/Item.hh"
#include "../Entity.hh"

namespace Attacks {
enum class AttackType {
    ARROW_PROJECTILE = 0,
    SWORD_SLASH = 1,
    MUSKET_BALL = 2,
};

struct WeaponInfo {
    WeaponInfo() = default;
    WeaponInfo(AttackType atkType);
    ~WeaponInfo() = default;

    EntityAttributes *attackSource;
    bool canHitOrigin = false;
    Uint32 canHitOriginAfter = -1;  // Number of ticks after being spawned

    Uint32 lifeEndTick = -1;  // Number of ticks after being spawned

    int timesHit = 0;
    int maxHits = 1;
    bool canHitTheSameEntityTwice = false;
    std::vector<std::weak_ptr<EntityAttributes>> entitiesHit = {};

    bool isEffectedByGravity = false;
};

class Arrow {
    WeaponInfo GetWeaponInfo();

   protected:
    WeaponInfo weaponInfo;
    Items::ItemStats itemStats;

    Quad<float> quad;
    float angle = 0;
    Vec2<float> maximumVelocity;

    bool isStuckToSurface = false;
    bool isStuckToEntity = false;  // if is stuck and not on entity is stuck on floor
    std::weak_ptr<EntityAttributes> stuckEntity;
    Vec2<int> posStuck;  // if stuck on entity this is the pos related to the entity

   protected:
    Arrow() = default;
    ~Arrow() = default;

   protected:
    // void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) override;

    void Init(Items::ItemStats itemStats_, EntityAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity);

    // void HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    // void HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    void HandleSurfaceCollision(const SDL_Rect &surfaceRect, const std::array<Vec2<int>, 4> &verticiesAttackRect);
    void HandleEntityCollision(std::shared_ptr<EntityAttributes> entity, const std::array<Vec2<int>, 4> &modelVerticies);

   private:
    void HandleQuadRotation();
    void HandleStuck();

    // void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override { (void)direction, (void)accelSpeed, (void)isPaused; };  // does nothing
};

//------------------------------------------------------------------------------

class Swing {  // I have plans to make most items have a swing attack, including ranged ones
   public:
    ~Swing() = default;

   protected:
    WeaponInfo weaponInfo;
    Items::ItemStats itemStats;
    float angle = 0;

    Quad<float> quad;

   protected:
    // std::shared_ptr<Swing> Create(const Items::ItemStats itemStats_, std::weak_ptr<Entity> entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions);

    Swing() = default;

   protected:
    void Init(const Items::ItemStats itemStats_, EntityAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions);

    // void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) override;

    // void HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

    // void HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;

   private:
    // void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override;

    void HandleEntityCollision(std::weak_ptr<EntityAttributes> entity);

    void HandleLifetime();
};

}  // namespace Attacks
