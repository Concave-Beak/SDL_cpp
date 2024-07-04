#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <algorithm>

#include "../../../../lib/utils/sdl_utils.hh"

namespace Attack {
WeaponInfo::WeaponInfo(AttackType atkType) {
    switch (atkType) {
        case AttackType::ARROW_PROJECTILE: {
            canHitOrigin = true;
            canHitOriginAfter = SDL_GetTicks() + 2000;

            lifeEndTick = -1;

            canHitTheSameEntityTwice = false;
            timesHit = 0;
            maxHits = 1;

            isEffectedByGravity = true;
            break;
        }
        case AttackType::SWORD_SLASH: {
            canHitOriginAfter = -1;

            lifeEndTick = -1;

            canHitTheSameEntityTwice = false;
            timesHit = 0;
            maxHits = 3;

            isEffectedByGravity = false;
            break;
        }
        case AttackType::MUSKET_BALL: {
            canHitOriginAfter = -1;

            lifeEndTick = -1;

            canHitTheSameEntityTwice = true;
            timesHit = 0;
            maxHits = 2;

            isEffectedByGravity = false;
            break;
        }
    }
}

Arrow::Arrow(Items::ItemStats itemStats_, Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity) : weaponInfo(AttackType::ARROW_PROJECTILE),
                                                                                                                                                         itemStats(itemStats_),
                                                                                                                                                         quad(positionNow_, dimentions),
                                                                                                                                                         angle(angle_),
                                                                                                                                                         maximumVelocity(velocity) {
    Entity::model = {0, 0, (int)dimentions.x, (int)dimentions.y};  // not really used
    Entity::velocityNow = {
        400 * cos(angle_),
        400 * sin(angle_),
    };
    Entity::positionNow = entityOrigin_->GetPos() + Vec2<int>{entityOrigin_->GetModel().w, entityOrigin_->GetModel().h} * 0.5f;
    Entity::type = EntityType::ARROW;

    weaponInfo.attackSource = entityOrigin_;
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
    if (weaponInfo.isEffectedByGravity == true) gravity = GRAVITY;

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
            (!weaponInfo.canHitOrigin && entity == weaponInfo.attackSource) ||
            (SDL_GetTicks() < weaponInfo.canHitOriginAfter && entity == weaponInfo.attackSource) ||
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
            entity->Damage(itemStats.damage);
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

Swing::Swing(const Items::ItemStats itemStats_, Entity *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions) : weaponInfo(AttackType::SWORD_SLASH),
                                                                                                                                         itemStats(itemStats_),
                                                                                                                                         quad(positionNow_, dimentions) {
    weaponInfo.attackSource = entityOrigin_;
    weaponInfo.lifeEndTick = SDL_GetTicks() + 200;

    Vec2<float> rectCenter = Vec2<float>{
        weaponInfo.attackSource->GetModel().x + weaponInfo.attackSource->GetModel().w / 2.0f,
        weaponInfo.attackSource->GetModel().y + weaponInfo.attackSource->GetModel().h / 2.0f,
    };
    float radius = sqrt(pow(weaponInfo.attackSource->GetModel().w * 0.6f, 2) + pow(weaponInfo.attackSource->GetModel().h * 0.6f, 2));
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
        weaponInfo.attackSource->GetModel().x + weaponInfo.attackSource->GetModel().w / 2.0f,
        weaponInfo.attackSource->GetModel().y + weaponInfo.attackSource->GetModel().h / 2.0f};

    float radius = sqrt(pow(weaponInfo.attackSource->GetModel().w * 0.6f, 2) + pow(weaponInfo.attackSource->GetModel().h * 0.6f, 2));

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
        // check to see if entity in entityVector is this and if can hit it's attacker
        if (entity == this->GetEntity() || (!weaponInfo.canHitOrigin && weaponInfo.attackSource == entity)) continue;
        if (std::find(weaponInfo.entitiesHit.begin(), weaponInfo.entitiesHit.end(), entity) != weaponInfo.entitiesHit.end() && !weaponInfo.canHitTheSameEntityTwice) continue;  // if it finds the entity in the entitiesHit vector and cannot hit it twice, it continues

        HandleEntityCollision(entity);
    }
}

void Swing::HandleEntityCollision(Entity *entity) {
    if (weaponInfo.maxHits <= weaponInfo.timesHit) {
        isMarkedForDeletion = true;
        return;
    }
    if (IsQuadColliding(quad, Quad<float>(entity->GetModel()))) {
        weaponInfo.timesHit++;
        entity->Damage(itemStats.damage);

        if (!weaponInfo.canHitTheSameEntityTwice) weaponInfo.entitiesHit.push_back(entity);
    }
}

void Swing::HandleLifetime() {
    if (SDL_GetTicks() > weaponInfo.lifeEndTick) isMarkedForDeletion = true;
}

void Swing::Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) {
    (void)direction, (void)accelSpeed, (void)isPaused;
}
}  // namespace Attack
