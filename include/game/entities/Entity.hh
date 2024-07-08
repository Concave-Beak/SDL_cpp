#pragma once

#include <SDL2/SDL_render.h>

#include <cstdlib>
#include <memory>
#include <vector>

#include "../../../lib/utils/engine_utils.hh"
#include "../../../lib/utils/math_utils.hh"
#include "../../main/Level.hh"

class Entity : std::enable_shared_from_this<Entity> {
    typedef std::vector<std::weak_ptr<Entity>> entityVec;

   public:
    static std::shared_ptr<Entity> Create();

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

    SDL_Rect GetModel();

    EntityType GetType();

    Direction GetFacingDirection();

    static void PushToEntities(std::weak_ptr<Entity> entity2Push);
    static std::vector<std::shared_ptr<Entity>> GetEntities();

    virtual void Move(const Direction& direction, const Vec2<float>& accelSpeed, const bool& isPaused);

    static void Handle(const float& timeDelta, const float& timeMultiplier, const bool& isPaused, const Vec2<int>& cameraPos, SDL_Renderer* renderer);

    void Damage(int damage);

    virtual ~Entity() = default;

   protected:
    Entity() = default;

   protected:
    bool collidedDown = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedUp = false;
    bool isAbovePlatform = true;

    EntityType type;
    Uint16 ID;

    Vec2<float> velocityNow = {0, 0};
    Vec2<float> positionNow = {0, 0};

    Direction facing = Direction::RIGHT;

    SDL_Rect model = {.x = (int)positionNow.x, .y = (int)positionNow.y, .w = 0, .h = 0};

    float surfaceAttrition = 0;

    float healthMax = 10;
    float heathNow = 10;
    bool isDead = false;

    bool isMarkedForDeletion = false;

   protected:
    void ResetCollisionState();

    std::shared_ptr<Entity> GetEntity();

   private:
    void UpdateModel();

    virtual void HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);
    virtual void HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused);

    void HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,  // TODO remove entityRect
                                 const float& timeDelta, const float& timeMultiplier);
    void HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                   const float& timeDelta, const float& timeMultiplier);
    virtual void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer);

    static void CheckExpiredEntities();
    static void Delete(entityVec::iterator entityIt);

   private:
    inline static entityVec entities = {};
};

//------------------------------------------------------------------------------
