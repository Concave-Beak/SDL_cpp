#include <SDL2/SDL_timer.h>

#include "../../../../include/game/entities/Attack/Attacks.hh"
#include "../../../../lib/utils/sdl_utils.hh"

namespace Attacks {
WeaponStats::WeaponStats(AttackType atkType) {
    switch (atkType) {
        case AttackType::ARROW_PROJECTILE: {
            canHitOrigin = true;
            canHitOriginAfter = SDL_GetTicks() + 2000;

            damage = 10;  // PLACEHOLDER

            lifeEndTick = -1;

            timesHit = 0;
            maxHits = 1;

            isEffectedByGravity = true;
            break;
        }
        case AttackType::SWORD_SLASH: {
            canHitOriginAfter = -1;

            damage = 10;  // PLACEHOLDER

            lifeEndTick = -1;

            timesHit = 0;
            maxHits = 3;

            isEffectedByGravity = false;
            break;
        }
        case AttackType::MUSKET_BALL: {
            canHitOriginAfter = -1;

            damage = 10;  // PLACEHOLDER

            lifeEndTick = -1;

            timesHit = 0;
            maxHits = 2;

            isEffectedByGravity = false;
            break;
        }
    }
}

void ArrowProjectile::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    SDL_Rect arrowModel = SDL_Rect{
        int(positionNow.x - cameraPos.x),
        int(positionNow.y - cameraPos.y),
        model.w,
        model.h};
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &arrowModel)).Handle();
}

void ArrowProjectile::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused || isStuckToSurface) return;

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
    if (weaponStats.isEffectedByGravity == true) gravity = GRAVITY;

    if (!collidedDown) {
        velocityNow.y += timeDelta * gravity * timeMultiplier;
        positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!collidedDown) surfaceAttrition = AIR_ATTRITION;  // resets attrition

    positionNow.x += velocityNow.x * timeMultiplier * timeDelta;
}

void ArrowProjectile::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused) return;
    if (isStuckToSurface || isStuckToEntity) {
        HandleStuck();
        return;
    }

    (void)timeDelta, (void)timeMultiplier;

    std::array<Vec2<int>, 4> modelVerticies = {
        Vec2<int>{(int)positionNow.x, (int)positionNow.y},
        Vec2<int>{(int)positionNow.x + model.w, (int)positionNow.y},
        Vec2<int>{(int)positionNow.x, (int)positionNow.y + model.h},
        Vec2<int>{(int)positionNow.x + model.w, (int)positionNow.y + model.h},
    };

    for (Entity *entity : Entity::entityVector) {
        if (entity == this->GetEntity() ||
            (!weaponStats.canHitOrigin && entity == weaponStats.entityOrigin) ||
            (SDL_GetTicks() < weaponStats.canHitOriginAfter && entity == weaponStats.entityOrigin) ||
            entity->GetType() == EntityType::ARROW) continue;

        HandleEntityCollision(entity, modelVerticies);
    }
    for (LevelItem levelItem : Level::collisions) {
        HandleSurfaceCollision(levelItem.rect, modelVerticies);
    }
}

void ArrowProjectile::HandleStuck() {
    if (isStuckToEntity && stuckEntity == nullptr) {
        isStuckToEntity = false;
        return;
    }
    if (isStuckToEntity) {
        positionNow = posStuck + stuckEntity->GetPos();
        return;
    }
    if (isStuckToSurface) {
        return;
    }
}

void ArrowProjectile::HandleEntityCollision(Entity *entity, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, entity->GetEntityRect())) {
            velocityNow = {0, 0};
            isStuckToEntity = true;
            stuckEntity = entity;
            posStuck = positionNow - stuckEntity->GetPos();
            entity->Damage(weaponStats.damage);
            return;
        }
    }
}

void ArrowProjectile::HandleSurfaceCollision(const SDL_Rect &surfaceRect, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, surfaceRect)) {
            velocityNow = {0, 0};
            isStuckToSurface = true;
            posStuck = positionNow;
            return;
        }
    }
}

}  // namespace Attacks
