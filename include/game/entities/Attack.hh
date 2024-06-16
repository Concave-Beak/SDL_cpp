#pragma once

#include <SDL2/SDL_stdinc.h>

#include "../../../lib/utils/math_utils.hh"
#include "Entity.hh"

class Attack : public Entity {
   public:
    enum AttackType {
        SWORD_SLASH = 0,
        ARROW,
    };
    Attack(Entity* entityOrigin, Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, float angle, Uint32 lifeTime);
    ~Attack();

    static void Handle();
    static void CheckAndDestroyExpiredAttacks();
    void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) override;

   private:
    Entity* entityOrigin = nullptr;

    Vec2<float> accelSpeed = {400, 400};

    float damage;

    AttackType atkType;

    Uint32 lifeEndTick;

    bool isEffectedByGravity = false;
    int timesHit = 0;
    int maxHits = 1;  // maximum amount of times it can hit before being deleted
    bool isMarkedForDeletion = false;

    inline static std::vector<Attack*> attackVector = {};

   private:
    void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) override;

    void HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) override;
    void HandleSurfaceCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, const SDL_Rect& entityRect);
    void HandleEntityCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, Entity* entity);

    // Delete by searching the attackVector
    void Delete();
    // or by passing the iterator
    void Delete(std::vector<Attack*>::iterator attackIt);

    void GetProjectileVelocity(Vec2<float> accelSpeed, float angle);
};
