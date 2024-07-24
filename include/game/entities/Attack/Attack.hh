#pragma once

#include <SDL2/SDL_render.h>

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
    Vec2<float> maximumVelocity;  // TODO: damage should be related to velocityNow and maximumVelocity

    bool isStuckToSurface = false;
    bool isStuckToEntity = false;                 // if is stuck and not on entity is stuck on floor
    mutable const EntityAttributes *stuckEntity;  // Can change which creature it's stuck on but not the creature attributes itself
    Vec2<int> posStuck;                           // if stuck on entity this is the pos related to the entity

   protected:
    Arrow() = default;
    ~Arrow() = default;

   protected:
    void Init(Items::ItemStats itemStats_, EntityAttributes *entityAttributes, float angle_, Vec2<int> positionNow_, int dimentions, Vec2<int> velocity);
};

class ArrowHandler {
   public:
    static void Handler(Attack *atk, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, SDL_Renderer *renderer);

   private:
    static void HandleVelocity(Arrow *arrow, float timeDelta, float timeMultiplier);

    static void HandleSurfaceCollision(Arrow *arrow, const SDL_Rect &surfaceRect);
    static void HandleCollisions(Arrow *arrow);
    static void HandleCreatureCollision(Arrow *arrow, Creatures::Creature *creatureStuck);
    static void HandlePlayerCollision(Arrow *arrow);

    static void UpdateModel(Arrow *arrow);

    static void HandleStuck(Arrow *arrow);

    static void Draw(const Arrow &arrow, Vec2<int> cameraPos, SDL_Renderer *renderer);
};

//------------------------------------------------------------------------------

class MeleeSwing : public Attack {  // I have plans to make most items have a swing attack, including ranged ones
    friend class MeleeSwingHandler;

   protected:
    ~MeleeSwing() = default;
    MeleeSwing() = default;

   protected:
    Items::ItemStats itemStats;

    int minDimention = 0;
    int maxDimention = 0;

    mutable const EntityAttributes *creatureOrigin = nullptr;  // this is used on UpdateModel

   protected:
    void Init(Items::ItemStats itemStats_, EntityAttributes *entityOrigin_, float angle_, Vec2<int> positionNow_, int minDimention, int maxDimention);
};

class MeleeSwingHandler {
   public:
    static void Handler(Attack *atk, Vec2<int> cameraPos, SDL_Renderer *renderer);

   private:
    static void HandleCollisions(MeleeSwing *swing);
    static void HandleCreatureCollision(MeleeSwing *swing, Creatures::Creature *creature);
    static void HandlePlayerCollision(MeleeSwing *swing, Creatures::Creature *creature);

    static void UpdateModel(MeleeSwing *swing);

    static void Draw(const MeleeSwing *attack, Vec2<int> cameraPos, SDL_Renderer *renderer);

    static void HandleLifetime(MeleeSwing *swing);
};

}  // namespace Attacks
