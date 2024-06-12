#include "../../../include/game/entities/Attack.hh"

#include <SDL2/SDL_timer.h>

#include "../../../lib/utils/sdl_utils.hh"

Attack::Attack(Entity* entityOrigin_, Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, Uint32 lifeTime)
    : entityOrigin(entityOrigin_), damage(damage_), atkType(atkType_), lifeEndTick(SDL_GetTicks() + lifeTime) {
    this->positionNow = spawnPos_;
    // switch (atkType_) {
    //     case AttackType::ARROW: {
    //         this->hitbox = {50, 10};
    //         break;
    //     }
    //     case AttackType::SWORD_SLASH: {
    //         this->hitbox = {30, 20};
    //         break;
    //     }
    //     default: {
    //         this->hitbox = {30, 10};
    //         break;
    //     }
    // }
    this->velocityNow = {80, 0};
    this->hitbox = {30, 10};
    this->Init(Entity::ARROW);
    attackVector.push_back(this);
}

Attack::~Attack() {
}

void Attack::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect attackModel = {
        int(this->positionNow.x - cameraPos.x),
        int(this->positionNow.y - cameraPos.y),
        this->hitbox.x,
        this->hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff));
    scc(SDL_RenderFillRect(renderer, &attackModel));
}

void Attack::CheckAndDestroyExpiredAttacks() {
    Uint32 currentTicks = SDL_GetTicks();
    for (std::vector<Attack*>::iterator attackIt = attackVector.begin(); attackIt != attackVector.end();) {
        if ((*attackIt)->lifeEndTick <= currentTicks) {
            Attack* attackToDelete = *attackIt;

            attackIt = attackVector.erase(attackIt);

            delete attackToDelete;

            for (std::vector<Entity*>::iterator entityIt = entityVector.begin(); entityIt != entityVector.end(); ++entityIt) {
                if (*entityIt == static_cast<Entity*>(attackToDelete)) {
                    entityVector.erase(entityIt);
                    break;
                }
            }
        } else {
            ++attackIt;
        }
    }
}

void Attack::HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    if (this->velocityNow.y > MAX_PROJECTYLE_SPEED_Y) {
        this->velocityNow.y = MAX_PROJECTYLE_SPEED_Y;
    }
    if (this->velocityNow.y < -MAX_PROJECTYLE_SPEED_Y) {
        this->velocityNow.y = -MAX_PROJECTYLE_SPEED_Y;
    }

    if (this->velocityNow.x > MAX_PROJECTILE_SPEED_X) {
        this->velocityNow.x = MAX_PROJECTILE_SPEED_X;
    }
    if (this->velocityNow.x < -MAX_PROJECTILE_SPEED_X) {
        this->velocityNow.x = -MAX_PROJECTILE_SPEED_X;
    }

    int gravity = 0;
    if (this->isEffectedByGravity) {
        gravity = GRAVITY;
    }

    if (this->collidedDown == false) {
        this->velocityNow.y += timeDelta * gravity * timeMultiplier;
        this->positionNow.y += this->velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!this->collidedDown)
        this->surfaceAttrition = AIR_ATTRITION;  // resets attrition

    this->velocityNow.x -= timeDelta * this->surfaceAttrition * this->velocityNow.x * timeMultiplier;
    this->positionNow.x += this->velocityNow.x * timeMultiplier;
}

void Attack::HandleVerticalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                     const float& timeDelta, const float& timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.wireframe.h);

    bool hitFeet = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::DOWN, this->velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        this->collidedDown = true;
        this->surfaceAttrition = levelItem.attritionCoefficient;
        this->positionNow.y = levelItemTop - float(this->hitbox.y);
        this->velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) this->isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::UP, this->velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        this->collidedUp = true;
        this->positionNow.y = levelItemBottom;
        this->velocityNow.y = -this->velocityNow.y * SURFACE_BOUNCE;
    }
}

void Attack::HandleHorizontalCollision(const SDL_Rect& entityRect, const LevelItem& levelItem,
                                       const float& timeDelta, const float& timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.wireframe.w);

    bool hitRight = CheckSideCollision(entityRect, levelItem.wireframe,
                                       Direction::RIGHT, this->velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        this->collidedRight = true;
        this->velocityNow.x = -this->velocityNow.x * SURFACE_BOUNCE;
        this->positionNow.x = levelItemLeft - float(this->hitbox.x);
    }

    bool hitLeft = CheckSideCollision(entityRect, levelItem.wireframe,
                                      Direction::LEFT, this->velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        this->collidedLeft = true;
        this->velocityNow.x = -this->velocityNow.x * SURFACE_BOUNCE;
        this->positionNow.x = levelItemRight;
    }
}

void Attack::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused)
        return;

    this->ResetCollisionState();

    SDL_Rect entityRect;
    for (LevelItem levelItem : Level::collisions) {
        entityRect = this->GetEntityRect();
        this->HandleVerticalCollision(entityRect, levelItem, timeDelta, timeMultiplier);
        entityRect = this->GetEntityRect();
        this->HandleHorizontalCollision(entityRect, levelItem, timeDelta, timeMultiplier);
    }
}
