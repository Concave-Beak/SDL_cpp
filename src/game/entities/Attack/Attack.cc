#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <cstdlib>
#include <functional>

#include "../../../../include/game/entities/Creature/Creature.hh"
#include "../../../../include/game/entities/Player.hh"
#include "../../../../lib/utils/sdl_utils.hh"

namespace Attacks {

void AttackHandler::PushToAttackVector(std::shared_ptr<Attack> atk) { attackVector.push_back(atk); }

void AttackHandler::InvokeHandler(Vec2<int> cameraPos, SDL_Renderer *renderer, float timeDelta, float timeMultiplier, bool isPaused) {
    for (AttackVector::iterator it = attackVector.begin(); it != attackVector.end();) {
        if ((*it)->atkAttributes.isMarkedForDeletion) {
            Delete(it);
            continue;
        }
        switch ((*it)->atkAttributes.attackType) {
            case AttackType::NOT_SET: {
                break;
            }
            case AttackType::ARROW_PROJECTILE: {
                ArrowHandler::Handler(it->get(), cameraPos, timeDelta, timeMultiplier, isPaused, renderer);
                break;
            }
            case AttackType::SWORD_SLASH: {
                break;
            }
            case AttackType::MUSKET_BALL: {
                break;
            }
        }
        ++it;
    }
}

AttackAttributes AttackHandler::GetWeaponInfo(Attack &atk) { return atk.atkAttributes; }

void AttackHandler::Delete(AttackVector::iterator it) {
    if (it != attackVector.end()) {
        attackVector.erase(it);
    }
}

//------------------------------------------------------------------------------

void Arrow::Init(Items::ItemStats itemStats_, CreatureAttributes *entityAttributes, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions, Vec2<float> velocity) {
    itemStats = itemStats_;
    atkAttributes.attackType = AttackType::ARROW_PROJECTILE;
    positionNow = positionNow_;
    atkAttributes.isEffectedByGravity = true;
    atkAttributes.model = Quad<float>(positionNow_, dimentions);
    atkAttributes.angle = angle_;
    maximumVelocity = velocity;
    velocityNow = {
        400 * cos(angle_) * itemStats.chargeNow / 100,
        400 * sin(angle_) * itemStats.chargeNow / 100,
    };

    atkAttributes.attackSourceID = entityAttributes->GetID();
    atkAttributes.model.RotateCenter(angle_);

    atkAttributes.canHitSource = true;
    atkAttributes.canHitSourceAfter = SDL_GetTicks() + 200;
}
void ArrowHandler::Handler(Attack *atk, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused, SDL_Renderer *renderer) {
    if (isPaused) return;

    Arrow *arrow = dynamic_cast<Arrow *>(atk);
    if (!arrow) return;

    HandleVelocity(arrow, timeDelta, timeMultiplier);
    HandleCollisions(arrow, timeDelta, timeMultiplier);
    HandleStuck(arrow);
    UpdateModel(arrow);
    Draw(arrow, cameraPos, renderer);
}

void ArrowHandler::UpdateModel(Arrow *arrow) {
    if (!arrow->isStuckToEntity && !arrow->isStuckToSurface) {
        arrow->atkAttributes.angle = atan2(arrow->velocityNow.y, arrow->velocityNow.x);
    }

    float angleDif = arrow->atkAttributes.angle - arrow->atkAttributes.model.GetAngle();
    arrow->atkAttributes.model.RotateCenter(angleDif);
}

void ArrowHandler::Draw(Arrow *arrow, Vec2<int> cameraPos, SDL_Renderer *renderer) {
    arrow->atkAttributes.model.Draw(renderer, cameraPos, {BLACK, 0xff});
}

void ArrowHandler::HandleVelocity(Arrow *arrow, float timeDelta, float timeMultiplier) {
    if (arrow->isStuckToSurface || arrow->isStuckToEntity) return;

    Vec2<float> *velocityPtr = &arrow->velocityNow;
    if (velocityPtr->y > MAX_PROJECTILE_SPEED_Y) {
        velocityPtr->y = MAX_PROJECTILE_SPEED_Y;
    }
    if (velocityPtr->y < -MAX_PROJECTILE_SPEED_Y) {
        velocityPtr->y = -MAX_PROJECTILE_SPEED_Y;
    }

    if (velocityPtr->x > MAX_PROJECTILE_SPEED_X) {
        velocityPtr->x = MAX_PROJECTILE_SPEED_X;
    }
    if (velocityPtr->x < -MAX_PROJECTILE_SPEED_X) {
        velocityPtr->x = -MAX_PROJECTILE_SPEED_X;
    }

    int gravity = 0;
    if (arrow->atkAttributes.isEffectedByGravity) gravity = GRAVITY;

    velocityPtr->y += timeDelta * gravity * timeMultiplier;
    arrow->positionNow.y += velocityPtr->y * timeDelta * timeMultiplier;
    arrow->positionNow.x += velocityPtr->x * timeMultiplier * timeDelta;

    arrow->atkAttributes.model.SetPos(arrow->positionNow, 0);
}

void ArrowHandler::HandleCollisions(Arrow *arrow, float timeDelta, float timeMultiplier) {
    if (arrow->isStuckToSurface || arrow->isStuckToEntity) {
        return;
    }

    (void)timeDelta, (void)timeMultiplier;

    HandlePlayerCollision(arrow, timeDelta, timeMultiplier);

    for (std::shared_ptr<Creatures::Creature> creature : Creatures::CreatureHandler::Instance().GetCreatureVector()) {
        std::function<bool()> shouldSkip = [arrow, creature]() -> bool {
            if (!arrow->atkAttributes.canHitSource && arrow->atkAttributes.attackSourceID == creature->GetID()) return true;

            if (arrow->atkAttributes.canHitSourceAfter < SDL_GetTicks()) return true;

            return false;
        };

        if (shouldSkip()) continue;

        HandleCreatureCollision(arrow, creature.get());
    }

    for (LevelItem levelItem : Level::collisions) {
        HandleSurfaceCollision(arrow, levelItem.rect);
    }
}

void ArrowHandler::HandleStuck(Arrow *attack) {
    if (attack->isStuckToEntity && attack->stuckEntity == nullptr) {
        attack->isStuckToEntity = false;
        return;
    }
    if (attack->isStuckToEntity) {
        attack->atkAttributes.model.SetPos(attack->posStuck + attack->stuckEntity->positionNow, 0);
        return;
    }
    if (attack->isStuckToSurface) {
        attack->atkAttributes.model.SetPos(attack->posStuck, 0);
        return;
    }
}

void ArrowHandler::HandleCreatureCollision(Arrow *arrow, Creatures::Creature *creatureStuck) {
    if (IsQuadColliding(arrow->atkAttributes.model, Quad<float>(creatureStuck->GetAttribute().model))) {
        arrow->velocityNow = {0, 0};
        arrow->isStuckToEntity = true;
        arrow->stuckEntity = creatureStuck->GetAttributeReference();
        arrow->posStuck = arrow->positionNow - creatureStuck->GetAttribute().positionNow;
        creatureStuck->Damage(arrow->itemStats.damage);
        return;
    }
}

void ArrowHandler::HandleSurfaceCollision(Arrow *arrow, const SDL_Rect &surfaceRect) {
    if (IsQuadColliding(arrow->atkAttributes.model, Quad<float>(surfaceRect))) {
        arrow->isStuckToSurface = true;
        arrow->posStuck = arrow->positionNow;
        arrow->velocityNow = {0, 0};
        return;
    }
}

void ArrowHandler::HandlePlayerCollision(Arrow *arrow, float timeDelta, float timeMultiplier) {
    if (arrow->atkAttributes.canHitSource && Player::GetAttribute().GetID() == arrow->atkAttributes.attackSourceID && arrow->atkAttributes.canHitSourceAfter < SDL_GetTicks()) {
        if (IsQuadColliding(arrow->atkAttributes.model, Quad<float>(Player::GetAttribute().model))) {
            arrow->isStuckToEntity = true;
            arrow->stuckEntity = Player::GetAttributeReference();
            arrow->posStuck = arrow->positionNow - Player::GetAttribute().positionNow;
        }
    }
}

//------------------------------------------------------------------------------

// void Swing::Init(Items::ItemStats itemStats_, CreatureAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions) {
//     weaponInfo = AttackType::SWORD_SLASH;
//     itemStats = Items::ItemStats(itemStats_);
//     quad = Quad<float>(positionNow_, dimentions);
//     weaponInfo.attackSourceID = entityOrigin_;
//     weaponInfo.lifeEndTick = SDL_GetTicks() + 200;
//
//     Vec2<float> rectCenter = Vec2<float>{
//         // weaponInfo.attackSource->GetModel().x + weaponInfo.attackSource->GetModel().w / 2.0f,
//         // weaponInfo.attackSource->GetModel().y + weaponInfo.attackSource->GetModel().h / 2.0f,
//     };
//     // float radius = sqrt(pow(weaponInfo.attackSource->GetModel().w * 0.6f, 2) + pow(weaponInfo.attackSource->GetModel().h * 0.6f, 2));
//     Vec2<float> spawnOffset = Vec2<float>{
//         // radius * cos(angle_),
//         // radius * sin(angle_),
//     };
//
//     Vec2<float> spawnPos = rectCenter + spawnOffset;
//
//     quad.SetPos(spawnPos, 4);
//     quad.RotateCenter(angle_);
//     angle = angle_;
// }

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
