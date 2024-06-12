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
    Attack(Entity* entityOrigin, Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, Uint32 lifeTime);
    ~Attack();

    static void Handle();
    static void CheckAndDestroyExpiredAttacks();
    void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) override;

   private:
    Entity* entityOrigin = nullptr;

    float damage;

    AttackType atkType;

    Uint32 lifeEndTick;

    bool isEffectedByGravity = false;

    inline static std::vector<Attack*> attackVector = {};

   private:
    void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) override;

    void HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) override;
    void HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                 const float& timeDelta, const float& timeMultiplier) override;
    void HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                   const float& timeDelta, const float& timeMultiplier) override;
};
