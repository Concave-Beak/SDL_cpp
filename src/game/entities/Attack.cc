#include "../../../include/game/entities/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <array>

#include "../../../lib/utils/sdl_utils.hh"

Attack::Attack(Entity* entityOrigin_, Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, float angle, Uint32 lifeTime)
    : entityOrigin(entityOrigin_), damage(damage_), atkType(atkType_), lifeEndTick(SDL_GetTicks() + lifeTime) {
    this->positionNow = spawnPos_;
    GetProjectileVelocity(accelSpeed, angle);
    this->hitbox = {10, 10};
    this->Init(Entity::ARROW);
    attackVector.push_back(this);
}

Attack::~Attack() {}

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
        if ((*attackIt)->lifeEndTick <= currentTicks || (*attackIt)->isMarkedForDeletion) {
            (*attackIt)->Delete(attackIt);
        } else {
            ++attackIt;
        }
    }
}

void Attack::HandleVelocity(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;

    if (this->velocityNow.y > MAX_PROJECTILE_SPEED_Y) {
        this->velocityNow.y = MAX_PROJECTILE_SPEED_Y;
    }
    if (this->velocityNow.y < -MAX_PROJECTILE_SPEED_Y) {
        this->velocityNow.y = -MAX_PROJECTILE_SPEED_Y;
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

    if (!this->collidedDown) {
        this->velocityNow.y += timeDelta * gravity * timeMultiplier;
        this->positionNow.y += this->velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!this->collidedDown) this->surfaceAttrition = AIR_ATTRITION;  // resets attrition

    // this->velocityNow.x -= timeDelta * this->surfaceAttrition * timeMultiplier * this->velocityNow.x;
    this->positionNow.x += this->velocityNow.x * timeMultiplier * timeDelta;
}

void Attack::HandleEntityCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, Entity* entity) {
    if (this->maxHits <= this->timesHit) this->isMarkedForDeletion = true;

    for (const Vec2<int>& point : verticiesAttackRect) {
        if (IsPointInRectangle(point, entity->GetEntityRect())) {
            this->isMarkedForDeletion = true;  // TODO: for now
            entity->Damage(this->damage);
            return;
        }
    }
}

void Attack::HandleSurfaceCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, const SDL_Rect& surfaceRect) {
    for (const Vec2<int>& point : verticiesAttackRect) {
        if (IsPointInRectangle(point, surfaceRect)) {
            this->isMarkedForDeletion = true;
            return;
        }
    }
}

void Attack::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    this->ResetCollisionState();

    SDL_Rect attackRect = this->GetEntityRect();
    SDL_Rect entityRect;

    std::array<Vec2<int>, 4> verticiesAttackRect = {
        Vec2<int>{attackRect.x, attackRect.y},
        Vec2<int>{attackRect.x + attackRect.w, attackRect.y},
        Vec2<int>{attackRect.x, attackRect.y + attackRect.h},
        Vec2<int>{attackRect.x + attackRect.w, attackRect.y + attackRect.h},
    };

    for (Entity* entity : Entity::entityVector) {
        if (entity == this->entityOrigin || entity == this->GetEntity()) continue;  // NOTE: I might change this depending if I want attacks/projectiles to hit each other

        this->HandleEntityCollision(verticiesAttackRect, entity);
        if (this->isMarkedForDeletion) return;
    }

    for (LevelItem levelItem : Level::collisions) {
        this->HandleSurfaceCollision(verticiesAttackRect, levelItem.wireframe);
        if (this->isMarkedForDeletion) return;
    }
}

void Attack::Delete() {
    for (std::vector<Attack*>::iterator attackIt = attackVector.begin(); attackIt != attackVector.end();) {
        if (this == *attackIt) {
            Attack* attackToDelete = *attackIt;

            attackIt = attackVector.erase(attackIt);

            delete attackToDelete;

            for (std::vector<Entity*>::iterator entityIt = entityVector.begin(); entityIt != entityVector.end(); ++entityIt) {
                if (*entityIt == static_cast<Entity*>(attackToDelete)) {
                    entityVector.erase(entityIt);
                    break;
                }
            }
            break;
        } else {
            ++attackIt;
        }
    }
}

void Attack::Delete(std::vector<Attack*>::iterator attackIt) {
    Attack* attackToDelete = *attackIt;

    attackIt = attackVector.erase(attackIt);

    delete attackToDelete;

    for (std::vector<Entity*>::iterator entityIt = entityVector.begin(); entityIt != entityVector.end(); ++entityIt) {
        if (*entityIt == static_cast<Entity*>(attackToDelete)) {
            entityVector.erase(entityIt);
            break;
        }
    }
}

void Attack::GetProjectileVelocity(Vec2<float> accelSpeed, float angle) {
    this->velocityNow.x = accelSpeed.x * cos(angle);
    this->velocityNow.y = accelSpeed.y * sin(angle);
}
