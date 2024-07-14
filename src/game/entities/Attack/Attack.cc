#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <cstdlib>

#include "../../../../lib/utils/sdl_utils.hh"

namespace Attacks {

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

void Arrow::Init(Items::ItemStats itemStats_, EntityAttributes *entityAttributes, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity) {
    weaponInfo = AttackType::ARROW_PROJECTILE;
    itemStats = itemStats_;
    quad = Quad<float>(positionNow_, dimentions);
    angle = angle_;
    maximumVelocity = velocity;
    // model = {0, 0, (int)dimentions.x, (int)dimentions.y};  // not really used
    // velocityNow = {
    //     400 * cos(angle_) * itemStats.chargeNow / 100,
    //     400 * sin(angle_) * itemStats.chargeNow / 100,
    // };

    // positionNow = entityOrigin_->GetPos() + Vec2<int>{entityOrigin_->GetModel().w, entityOrigin_->GetModel().h} * 0.5f;
    // type = EntityType::ARROW;

    weaponInfo.attackSource = entityAttributes;
    quad.RotateCenter(angle_);
}

void Arrow::HandleQuadRotation() {
    // angle = atan2(velocityNow.y, velocityNow.x);

    float angleDif = angle - quad.GetAngle();
    quad.RotateCenter(angleDif);
}

// void Arrow::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
//     quad.Draw(renderer, cameraPos, {BLACK, 0xff});
// }

// void Arrow::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
//     if (isPaused || isStuckToSurface || isStuckToEntity) return;
//     HandleQuadRotation();
//
//     if (velocityNow.y > MAX_PROJECTILE_SPEED_Y) {
//         velocityNow.y = MAX_PROJECTILE_SPEED_Y;
//     }
//     if (velocityNow.y < -MAX_PROJECTILE_SPEED_Y) {
//         velocityNow.y = -MAX_PROJECTILE_SPEED_Y;
//     }
//
//     if (velocityNow.x > MAX_PROJECTILE_SPEED_X) {
//         velocityNow.x = MAX_PROJECTILE_SPEED_X;
//     }
//     if (velocityNow.x < -MAX_PROJECTILE_SPEED_X) {
//         velocityNow.x = -MAX_PROJECTILE_SPEED_X;
//     }
//
//     int gravity = 0;
//     if (weaponInfo.isEffectedByGravity) gravity = GRAVITY;
//
//     velocityNow.y += timeDelta * gravity * timeMultiplier;
//     positionNow.y += velocityNow.y * timeDelta * timeMultiplier;
//     positionNow.x += velocityNow.x * timeMultiplier * timeDelta;
//
//     quad.SetPos(positionNow, 0);
// }

// void Arrow::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
//     if (isPaused) return;
//     if (isStuckToSurface || isStuckToEntity) {
//         HandleStuck();
//         return;
//     }
//
//     (void)timeDelta, (void)timeMultiplier;
//
//     std::array<Vec2<int>, 4> modelVerticies = {
//         Vec2<int>(quad.a),
//         Vec2<int>(quad.b),
//         Vec2<int>(quad.c),
//         Vec2<int>(quad.d),
//     };
//
//     for (std::shared_ptr<Entity> entity : Entity::GetEntities()) {
//         if (entity.get() == this->GetEntity() ||
//             (!weaponInfo.canHitOrigin && entity.get() == weaponInfo.attackSource) ||
//             (SDL_GetTicks() < weaponInfo.canHitOriginAfter && entity.get() == weaponInfo.attackSource) ||
//             entity->GetType() == EntityType::ARROW) continue;
//
//         HandleEntityCollision(entity, modelVerticies);
//     }
//     for (LevelItem levelItem : Level::collisions) {
//         HandleSurfaceCollision(levelItem.rect, modelVerticies);
//     }
// }

void Arrow::HandleStuck() {
    if (isStuckToEntity && stuckEntity.expired()) {
        isStuckToEntity = false;
        return;
    }
    if (isStuckToEntity) {
        quad.SetPos(posStuck + stuckEntity.lock()->positionNow, 0);
        return;
    }
    if (isStuckToSurface) {
        quad.SetPos(posStuck, 0);
        return;
    }
}

void Arrow::HandleEntityCollision(std::shared_ptr<EntityAttributes> entityAttributes, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, entityAttributes->model)) {
            entityAttributes->velocityNow = {0, 0};
            isStuckToEntity = true;
            stuckEntity = entityAttributes;
            posStuck = entityAttributes->positionNow - stuckEntity.lock()->positionNow;
            // entityAttributes->Damage(itemStats.damage);
            return;
        }
    }
}

void Arrow::HandleSurfaceCollision(const SDL_Rect &surfaceRect, const std::array<Vec2<int>, 4> &modelVerticies) {
    for (const Vec2<int> &point : modelVerticies) {
        if (IsPointInRectangle(point, surfaceRect)) {
            // velocityNow = {0, 0};
            isStuckToSurface = true;
            // posStuck = positionNow;
            return;
        }
    }
}

//------------------------------------------------------------------------------

void Swing::Init(Items::ItemStats itemStats_, EntityAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions) {
    weaponInfo = AttackType::SWORD_SLASH;
    itemStats = Items::ItemStats(itemStats_);
    quad = Quad<float>(positionNow_, dimentions);
    weaponInfo.attackSource = entityOrigin_;
    weaponInfo.lifeEndTick = SDL_GetTicks() + 200;

    Vec2<float> rectCenter = Vec2<float>{
        // weaponInfo.attackSource->GetModel().x + weaponInfo.attackSource->GetModel().w / 2.0f,
        // weaponInfo.attackSource->GetModel().y + weaponInfo.attackSource->GetModel().h / 2.0f,
    };
    // float radius = sqrt(pow(weaponInfo.attackSource->GetModel().w * 0.6f, 2) + pow(weaponInfo.attackSource->GetModel().h * 0.6f, 2));
    Vec2<float> spawnOffset = Vec2<float>{
        // radius * cos(angle_),
        // radius * sin(angle_),
    };

    Vec2<float> spawnPos = rectCenter + spawnOffset;

    quad.SetPos(spawnPos, 4);
    quad.RotateCenter(angle_);
    angle = angle_;
}

// void Swing::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
//     quad.Draw(renderer, cameraPos, {BLACK, 0xff});
// }

// void Swing::HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
//     if (isPaused) return;
//     if (isMarkedForDeletion) return;
//     (void)timeDelta, (void)timeMultiplier;
//
//     Vec2<float> rectCenter = Vec2<float>{
//         weaponInfo.attackSource->GetModel().x + weaponInfo.attackSource->GetModel().w / 2.0f,
//         weaponInfo.attackSource->GetModel().y + weaponInfo.attackSource->GetModel().h / 2.0f};
//
//     float radius = sqrt(pow(weaponInfo.attackSource->GetModel().w * 0.6f, 2) + pow(weaponInfo.attackSource->GetModel().h * 0.6f, 2));
//
//     Vec2<float> spawnOffset = Vec2<float>{
//         radius * cos(angle),
//         radius * sin(angle)};
//
//     Vec2<float> spawnPos = rectCenter + spawnOffset;
//     quad.SetPos(spawnPos, 4);
//
//     HandleLifetime();
// }

// void Swing::HandleCollisions(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) {
//     if (isPaused) return;
//     if (isMarkedForDeletion) return;
//     (void)timeDelta, (void)timeMultiplier;
//
//     for (std::weak_ptr<Entity> entity : Entity::GetEntities()) {
//         std::function<bool()> ShouldSkip = [entity, this]() -> bool {
//             // check to see if entity in entityVector is this and if can hit it's attacker
//             if (entity.lock().get() == this->GetEntity() || (!weaponInfo.canHitOrigin && weaponInfo.attackSource == entity.lock().get())) return true;
//
//             // check to see if can hit the same entity twice, if not it checks if the entity being hit is the same
//             if (weaponInfo.canHitTheSameEntityTwice) return false;
//             for (std::vector<std::weak_ptr<Entity>>::iterator it = weaponInfo.entitiesHit.begin(); it != weaponInfo.entitiesHit.end(); ++it) {
//                 if ((*it).lock() == entity.lock()) {
//                     return true;
//                 }
//             }
//             return false;
//         };
//         if (ShouldSkip()) continue;
//
//         HandleEntityCollision(entity.lock());
//     }
// }

// void Swing::HandleEntityCollision(std::weak_ptr<Entity> entity) {
//     if (weaponInfo.maxHits <= weaponInfo.timesHit) {
//         isMarkedForDeletion = true;
//         return;
//     }
//     if (IsQuadColliding(quad, Quad<float>(entity.lock()->GetModel()))) {
//         weaponInfo.timesHit++;
//         entity.lock()->Damage(itemStats.damage);
//
//         if (!weaponInfo.canHitTheSameEntityTwice && entity.lock() != nullptr) weaponInfo.entitiesHit.push_back(entity);
//     }
// }

// void Swing::HandleLifetime() {
//     if (SDL_GetTicks() > weaponInfo.lifeEndTick) isMarkedForDeletion = true;
// }

// void Swing::Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) {
//     (void)direction, (void)accelSpeed, (void)isPaused;
// }
}  // namespace Attacks
