#pragma once

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>

#include <cstdint>
#include <cstdlib>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"

struct EntityAttributes {
    EntityAttributes();
    ~EntityAttributes() = default;

    Vec2<float> walkingSpeed = {0, 0};
    Vec2<float> runningSpeed = {0, 0};

    Vec2<float> velocityNow = {0, 0};

    Vec2<float> positionNow = {0, 0};
    Vec2<int> spawnPos = {0, 0};

    Direction facing = Direction::RIGHT;

    SDL_Rect model = {0, 0, 1000, 1000};  // it's huge just so it's noticeable if not set

    Uint32 GetID();

    struct CombatAttributes {
        Uint32 itemUseCooldown = 0;

        float healthMax = 10;
        float heathNow = 10;

        bool isDead = false;
    } combatAttributes;

    struct CollisionAttributes {
        bool collidedDown = false;
        bool collidedLeft = false;
        bool collidedRight = false;
        bool collidedUp = false;
        bool isAbovePlatform = true;

        float surfaceAttrition = 0;
    } collisionAttributes;

   private:
    Uint32 id;

    inline static Uint32 maxUsedID = 0;
};

enum class CreatureType {
    THIEF = 0,
};

struct CreatureAttributes {
    CreatureType type;

    uint32_t isAggressiveToFlag;  // bitmaks for the CreatureType
    bool isAggressiveToPlayer;

    bool isAggroed = false;
    const mutable EntityAttributes* isAggroedTo;

    bool isMarkedForDeletion = false;
};

enum class AttackType {
    NOT_SET = 0,

    ARROW_PROJECTILE = 0x1,

    SWORD_SLASH = 0x20,

    MUSKET_BALL = 0x40,
};

struct AttackAttributes {
    AttackAttributes();
    ~AttackAttributes() = default;

    AttackType attackType = AttackType::NOT_SET;

    Uint32 attackSourceID = -1;  // Loops back to uint_32 max so it doesnt work if not set
    bool canHitSource = false;
    Uint32 canHitSourceAfter = -1;  // Number of ticks after being spawned

    Uint32 lifeEndTick = -1;  // Number of ticks after being spawned

    int timesHit = 0;
    int maxHits = 1;

    bool isEffectedByGravity = false;

    bool isMarkedForDeletion = false;

    SDL_Rect model;
    float angle;

   private:
    Uint64 id;
    inline static Uint64 maxUsedID = 0;
};

//------------------------------------------------------------------------------
