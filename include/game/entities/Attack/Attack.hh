#pragma once

#include <SDL2/SDL_render.h>

#include <array>
#include <memory>

#include "../../../../lib/utils/math_utils.hh"
#include "../../items/Item.hh"
#include "../Creature/Creature.hh"
#include "../EntityAttributes.hh"

namespace Attacks {

class Attack {
    friend class AttackHandler;

   public:
    virtual ~Attack() = default;

   protected:
    Attack() = default;

    AttackAttributes atkAttributes;
};

class AttackHandler {
    typedef std::vector<std::shared_ptr<Attack>> AttackVector;

   public:
    virtual ~AttackHandler() = default;

    static void PushToAttackVector(std::shared_ptr<Attack> atk);

   public:
    static void InvokeHandler(Vec2<int> cameraPos, SDL_Renderer *renderer, float timeDelta, float timeMultiplier, bool isPaused);

   public:
    AttackAttributes GetWeaponInfo(Attack &atk);

   protected:
    static void Delete(AttackVector::iterator attack);

   protected:
    static inline AttackVector attackVector;

   private:
    AttackHandler() = default;
};

//------------------------------------------------------------------------------

class Arrow : public Attack {
    friend class ArrowHandler;

   protected:
    Items::ItemStats itemStats;

    Vec2<float> positionNow;  // Just to make processing physics easier

    Vec2<float> velocityNow;
    Vec2<float> maximumVelocity;

    bool isStuckToSurface = false;
    bool isStuckToEntity = false;                   // if is stuck and not on entity is stuck on floor
    mutable const CreatureAttributes *stuckEntity;  // Can change which creature it's stuck on but not the creature attributes itself
    Vec2<int> posStuck;                             // if stuck on entity this is the pos related to the entity

   protected:
    Arrow() = default;
    ~Arrow() = default;

   protected:
    void Init(Items::ItemStats itemStats_, CreatureAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity);
};

class ArrowHandler : public AttackHandler {
   public:
    static void Handler(Attack *atk, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused, SDL_Renderer *renderer);

   private:
    static void HandleVelocity(Arrow *arrow, float timeDelta, float timeMultiplier);

    static void HandleSurfaceCollision(Arrow *arrow, const SDL_Rect &surfaceRect);
    static void HandleCreatureCollision(Arrow *arrow, Creatures::Creature *creatureStuck);
    static void HandleCollisions(Arrow *arrow, float timeDelta, float timeMultiplier);
    static void HandlePlayerCollision(Arrow *arrow, float timeDelta, float timeMultiplier);

    static void UpdateModel(Arrow *arrow);

    static void HandleStuck(Arrow *arrow);

    static void Draw(Arrow *arrow, Vec2<int> cameraPos, SDL_Renderer *renderer);
};

//------------------------------------------------------------------------------

class Swing {  // I have plans to make most items have a swing attack, including ranged ones
   public:
    ~Swing() = default;

   protected:
    AttackAttributes weaponInfo;
    Items::ItemStats itemStats;

   protected:
    // std::shared_ptr<Swing> Create(const Items::ItemStats itemStats_, std::weak_ptr<Entity> entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions);

    Swing() = default;

   protected:
    void Init(const Items::ItemStats itemStats_, CreatureAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions);

   private:
    // void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override;

    void HandleEntityCollision(std::weak_ptr<CreatureAttributes> entity);

    void HandleLifetime();
};

class SwingHandler : public AttackHandler {
    // void Draw(const Attack &attack, Vec2<int> cameraPos) override;

    // void HandleVelocity(Attack &attack, float timeDelta, float timeMultiplier, bool isPaused) override;
    //
    // void HandleCollisions(Attack &attack, float timeDelta, float timeMultiplier, bool isPaused) override;
    //
    // void UpdateModel(Attack &attack) override;
};

}  // namespace Attacks
