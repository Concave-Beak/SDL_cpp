#include "../../../include/game/entities/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <array>

#include "../../../lib/utils/sdl_utils.hh"

Attack::Attack(Entity* entityOrigin_, Uint32 damage_, Vec2<float> spawnPos_, AttackType atkType_, float angle, Uint32 lifeTime)
    : entityOrigin(entityOrigin_), damage(damage_), atkType(atkType_), lifeEndTick(SDL_GetTicks() + lifeTime) {
    positionNow = spawnPos_;
    GetProjectileVelocity(accelSpeed, angle);
    hitbox = {10, 10};
    Init(Entity::ARROW);
    attackVector.push_back(this);
}

Attack::~Attack() {}

void Attack::Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) {
    SDL_Rect attackModel = {
        int(positionNow.x - cameraPos.x),
        int(positionNow.y - cameraPos.y),
        hitbox.x,
        hitbox.y,
    };
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &attackModel)).Handle();
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

    if (velocityNow.y > MAX_PROJECTILE_SPEED_Y) {
        velocityNow.y = MAX_PROJECTILE_SPEED_Y;
    }
    if (velocityNow.y < -MAX_PROJECTILE_SPEED_Y) {
        velocityNow.y = -MAX_PROJECTILE_SPEED_Y;
    }

    if (velocityNow.x > MAX_PROJECTILE_SPEED_X) {
        velocityNow.x = MAX_PROJECTILE_SPEED_X;
    }
    if (velocityNow.x < -MAX_PROJECTILE_SPEED_X) {
        velocityNow.x = -MAX_PROJECTILE_SPEED_X;
    }

    int gravity = 0;
    if (isEffectedByGravity) {
        gravity = GRAVITY;
    }

    if (!collidedDown) {
        velocityNow.y += timeDelta * gravity * timeMultiplier;
        positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!collidedDown) surfaceAttrition = AIR_ATTRITION;  // resets attrition

    // this->velocityNow.x -= timeDelta * this->surfaceAttrition * timeMultiplier * this->velocityNow.x; // TODO Make it's speed goes down when colliding
    positionNow.x += velocityNow.x * timeMultiplier * timeDelta;
}

void Attack::HandleEntityCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, Entity* entity) {  // TODO make it not colide with the same entity twice
    if (maxHits <= timesHit) isMarkedForDeletion = true;

    for (const Vec2<int>& point : verticiesAttackRect) {
        if (IsPointInRectangle(point, entity->GetEntityRect())) {
            isMarkedForDeletion = true;  // TODO: for now
            entity->Damage(damage);
            return;
        }
    }
}

void Attack::HandleSurfaceCollision(const std::array<Vec2<int>, 4>& verticiesAttackRect, const SDL_Rect& surfaceRect) {
    for (const Vec2<int>& point : verticiesAttackRect) {
        if (IsPointInRectangle(point, surfaceRect)) {
            isMarkedForDeletion = true;
            return;
        }
    }
}

void Attack::HandleCollisions(const float& timeDelta, const float& timeMultiplier, const bool& isPaused) {
    if (isPaused) return;
    (void)timeDelta;
    (void)timeMultiplier;
    ResetCollisionState();

    SDL_Rect attackRect = GetEntityRect();

    std::array<Vec2<int>, 4> verticiesAttackRect = {
        Vec2<int>{attackRect.x, attackRect.y},
        Vec2<int>{attackRect.x + attackRect.w, attackRect.y},
        Vec2<int>{attackRect.x, attackRect.y + attackRect.h},
        Vec2<int>{attackRect.x + attackRect.w, attackRect.y + attackRect.h},
    };

    for (Entity* entity : Entity::entityVector) {
        if (entity == entityOrigin || entity == GetEntity()) continue;  // NOTE: I might change this depending if I want attacks/projectiles to hit each other

        HandleEntityCollision(verticiesAttackRect, entity);
        if (isMarkedForDeletion) return;
    }

    for (LevelItem levelItem : Level::collisions) {
        HandleSurfaceCollision(verticiesAttackRect, levelItem.wireframe);
        if (isMarkedForDeletion) return;
    }
}

void Attack::Delete() {
    for (std::vector<Attack*>::iterator attackIt = attackVector.begin(); attackIt != attackVector.end();) {
        if (*attackIt) {
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
    velocityNow.x = accelSpeed.x * cos(angle);
    velocityNow.y = accelSpeed.y * sin(angle);
}
