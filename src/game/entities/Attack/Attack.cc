#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <iostream>

#include "../../../../lib/utils/sdl_utils.hh"

namespace Attack {
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

Arrow::Arrow(Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity) : weaponStats(AttackType::ARROW_PROJECTILE), quad(positionNow_, dimentions), angle(angle_), maximumVelocity(velocity) {
    Entity::model = {0, 0, (int)dimentions.x, (int)dimentions.y};  // not really used
    Entity::velocityNow = {
        400 * cos(angle_),
        400 * sin(angle_),
    };
    Entity::positionNow = entityOrigin_->GetPos() + Vec2<int>{entityOrigin_->GetModel().w, entityOrigin_->GetModel().h} * 0.5f;
    Entity::type = EntityType::ARROW;

    weaponStats.entityOrigin = entityOrigin_;
    quad.RotateCenter(angle_);
}

void Arrow::HandleQuadRotation() {
    angle = atan2(velocityNow.y, velocityNow.x);

    float angleDif = angle - quad.GetAngle();
    quad.RotateCenter(angleDif);
}

void Arrow::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    quad.Draw(renderer, cameraPos, {BLACK, 0xff});
}

void Arrow::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
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

    velocityNow.y += timeDelta * gravity * timeMultiplier;
    positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
    positionNow.x += velocityNow.x * timeMultiplier * timeDelta;

    quad.SetPos(positionNow, 0);
}

void Arrow::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
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

void Arrow::HandleStuck() {
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

void Arrow::HandleEntityCollision(Entity *entity, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, entity->GetModel())) {
            velocityNow = {0, 0};
            isStuckToEntity = true;
            stuckEntity = entity;
            posStuck = positionNow - stuckEntity->GetPos();
            entity->Damage(weaponStats.damage);
            return;
        }
    }
}

void Arrow::HandleSurfaceCollision(const SDL_Rect &surfaceRect, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, surfaceRect)) {
            velocityNow = {0, 0};
            isStuckToSurface = true;
            posStuck = positionNow;
            return;
        }
    }
}

//------------------------------------------------------------------------------

Swing::Swing(Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions) : weaponStats(AttackType::SWORD_SLASH), quad(positionNow_, dimentions) {
    weaponStats.entityOrigin = entityOrigin_;
    weaponStats.lifeEndTick = SDL_GetTicks() + 200;

    Vec2<float> rectCenter = Vec2<float>{
        weaponStats.entityOrigin->GetModel().x + weaponStats.entityOrigin->GetModel().w / 2.0f,
        weaponStats.entityOrigin->GetModel().y + weaponStats.entityOrigin->GetModel().h / 2.0f};
    float radius = sqrt(pow(weaponStats.entityOrigin->GetModel().w * 0.6f, 2) + pow(weaponStats.entityOrigin->GetModel().h * 0.6f, 2));
    Vec2<float> spawnOffset = Vec2<float>{
        radius * cos(angle_),
        radius * sin(angle_)};

    Vec2<float> spawnPos = rectCenter + spawnOffset;

    quad.SetPos(spawnPos, 4);
    quad.RotateCenter(angle_);
    angle = angle_;
}

void Swing::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    quad.Draw(renderer, cameraPos, {BLACK, 0xff});
}

void Swing::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused) return;
    if (isMarkedForDeletion) return;
    (void)timeDelta, (void)timeMultiplier;

    Vec2<float> rectCenter = Vec2<float>{
        weaponStats.entityOrigin->GetModel().x + weaponStats.entityOrigin->GetModel().w / 2.0f,
        weaponStats.entityOrigin->GetModel().y + weaponStats.entityOrigin->GetModel().h / 2.0f};
    float radius = sqrt(pow(weaponStats.entityOrigin->GetModel().w * 0.6f, 2) + pow(weaponStats.entityOrigin->GetModel().h * 0.6f, 2));
    Vec2<float> spawnOffset = Vec2<float>{
        radius * cos(angle),
        radius * sin(angle)};

    Vec2<float> spawnPos = rectCenter + spawnOffset;
    quad.SetPos(spawnPos, 4);

    HandleLifetime();
}

void Swing::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
    if (isPaused) return;
    if (isMarkedForDeletion) return;
    (void)timeDelta, (void)timeMultiplier;

    for (Entity *entity : entityVector) {
        if (entity == this->GetEntity() || (!weaponStats.canHitOrigin && weaponStats.entityOrigin == entity)) continue;

        HandleEntityCollision(entity);
    }
}

void Swing::HandleEntityCollision(Entity *entity) {
    if (IsQuadColliding(quad, Quad<float>(entity->GetModel()))) {
        weaponStats.timesHit++;
        entity->Damage(weaponStats.damage);
        return;
    }
}

void Swing::HandleLifetime() {
    if (SDL_GetTicks() > weaponStats.lifeEndTick) isMarkedForDeletion = true;
}

void Swing::Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) {
    (void)direction, (void)accelSpeed, (void)isPaused;
}
}  // namespace Attack
