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

ArrowProjectile::ArrowProjectile(float angle_, Vec2<float> posNow_, Vec2<float> dimentions, Vec2<float> velocity) : weaponStats(AttackType::ARROW_PROJECTILE), quad(posNow_, dimentions), angle(angle_), maximumVelocity(velocity) {
    velocityNow = {
        400 * cos(angle_),
        400 * sin(angle_),
    };
    quad.RotateCenter(angle_);
}

void ArrowProjectile::HandleQuadRotation() {
    angle = atan2(velocityNow.y, velocityNow.x);

    float angleDif = angle - quad.GetAngle();
    quad.RotateCenter(angleDif);
}

void ArrowProjectile::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    quad.Draw(renderer, cameraPos, {BLACK, 0xff});
}

void ArrowProjectile::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused || isStuckToSurface || isStuckToEntity) return;
    HandleQuadRotation();

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

    velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
    positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
    positionNow.x += velocityNow.x * timeMultiplier * timeDelta;

    quad.SetPos(positionNow, 0);
}

void ArrowProjectile::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused) return;
    if (isStuckToSurface || isStuckToEntity) {
        HandleStuck();
        return;
    }

    (void)timeDelta, (void)timeMultiplier;

    std::array<Vec2<int>, 4> modelVerticies = {
        Vec2<int>(quad.a),
        Vec2<int>(quad.b),
        Vec2<int>(quad.c),
        Vec2<int>(quad.d),
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
        quad.SetPos(posStuck + stuckEntity->GetPos(), 0);
        return;
    }
    if (isStuckToSurface) {
        quad.SetPos(posStuck, 0);
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
