#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_timer.h>

#include <functional>
#include <memory>
#include <vector>

#include "../../../../include/game/entities/Creature/Creature.hh"
#include "../../../../include/game/entities/Player.hh"
#include "../../../../lib/utils/sdl_utils.hh"

namespace Attacks {

void AttackHandler::PushToAttackVector(std::shared_ptr<Attack> atk) { attackVector.push_back(atk); }

void AttackHandler::InvokeHandler(Vec2<int> cameraPos, SDL_Renderer *renderer, float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

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
                ArrowHandler::Handler(it->get(), cameraPos, timeDelta, timeMultiplier, renderer);
                break;
            }
            case AttackType::SWORD_SLASH: {
                MeleeSwingHandler::Handler(it->get(), cameraPos, renderer);
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
void ArrowHandler::Handler(Attack *atk, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, SDL_Renderer *renderer) {
    Arrow *arrow = dynamic_cast<Arrow *>(atk);
    if (!arrow) return;

    HandleVelocity(arrow, timeDelta, timeMultiplier);
    HandleCollisions(arrow);
    HandleStuck(arrow);
    UpdateModel(arrow);
    Draw(*arrow, cameraPos, renderer);
}

void ArrowHandler::UpdateModel(Arrow *arrow) {
    if (!arrow->isStuckToEntity && !arrow->isStuckToSurface) {
        arrow->atkAttributes.angle = atan2(arrow->velocityNow.y, arrow->velocityNow.x);
    }

    float angleDif = arrow->atkAttributes.angle - arrow->atkAttributes.model.GetAngle();
    arrow->atkAttributes.model.RotateCenter(angleDif);
}

void ArrowHandler::Draw(const Arrow &arrow, Vec2<int> cameraPos, SDL_Renderer *renderer) {
    arrow.atkAttributes.model.Draw(renderer, cameraPos, {BLACK, 0xff});
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

void ArrowHandler::HandleCollisions(Arrow *arrow) {
    if (arrow->isStuckToSurface || arrow->isStuckToEntity) {
        return;
    }

    HandlePlayerCollision(arrow);

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

void ArrowHandler::HandlePlayerCollision(Arrow *arrow) {
    if (arrow->atkAttributes.canHitSource && Player::GetAttribute().GetID() == arrow->atkAttributes.attackSourceID && arrow->atkAttributes.canHitSourceAfter < SDL_GetTicks()) {
        if (IsQuadColliding(arrow->atkAttributes.model, Quad<float>(Player::GetAttribute().model))) {
            arrow->isStuckToEntity = true;
            arrow->stuckEntity = Player::GetAttributeReference();
            arrow->posStuck = arrow->positionNow - Player::GetAttribute().positionNow;
        }
    }
}

//------------------------------------------------------------------------------

void MeleeSwing::Init(Items::ItemStats itemStats_, CreatureAttributes *entityOrigin_, float angle_, Vec2<float> positionNow_, Vec2<float> dimentions) {
    atkAttributes.attackType = AttackType::SWORD_SLASH;
    itemStats = Items::ItemStats(itemStats_);
    atkAttributes.model = Quad<float>(positionNow_, dimentions);
    atkAttributes.attackSourceID = entityOrigin_->GetID();
    atkAttributes.lifeEndTick = SDL_GetTicks() + 200;

    atkAttributes.canHitSource = false;
    atkAttributes.canHitSourceAfter = -1;
    creatureOrigin = entityOrigin_;

    Vec2<float> rectCenter = Vec2<float>{
        entityOrigin_->model.x + entityOrigin_->model.w / 2.0f,
        entityOrigin_->model.y + entityOrigin_->model.h / 2.0f,
    };

    float radius = sqrt(pow(entityOrigin_->model.w * 0.6f, 2) + pow(entityOrigin_->model.h * 0.6f, 2));
    Vec2<float> spawnOffset = Vec2<float>{
        radius * cos(angle_),
        radius * sin(angle_),
    };

    Vec2<float> spawnPos = rectCenter + spawnOffset;

    atkAttributes.model.SetPos(spawnPos, 4);
    atkAttributes.model.RotateCenter(angle_);
    atkAttributes.angle = angle_;
}

void MeleeSwingHandler::Handler(Attack *atk, Vec2<int> cameraPos, SDL_Renderer *renderer) {
    MeleeSwing *swing = dynamic_cast<MeleeSwing *>(atk);
    if (!swing) return;

    HandleCollisions(swing);
    UpdateModel(swing);
    Draw(swing, cameraPos, renderer);
    HandleLifetime(swing);
}

void MeleeSwingHandler::Draw(const MeleeSwing *swing, Vec2<int> cameraPos, SDL_Renderer *renderer) {
    swing->atkAttributes.model.Draw(renderer, cameraPos, {BLACK, 0xff});
}

void MeleeSwingHandler::UpdateModel(MeleeSwing *swing) {
    if (swing->atkAttributes.isMarkedForDeletion) return;

    Vec2<float> rectCenter = Vec2<float>{
        swing->creatureOrigin->model.x + swing->creatureOrigin->model.w / 2.0f,
        swing->creatureOrigin->model.y + swing->creatureOrigin->model.h / 2.0f,
    };

    float radius = sqrt(pow(swing->creatureOrigin->model.w * 0.6f, 2) + pow(swing->creatureOrigin->model.h * 0.6f, 2));
    Vec2<float> spawnOffset = Vec2<float>{
        radius * cos(swing->atkAttributes.angle),
        radius * sin(swing->atkAttributes.angle),
    };

    Vec2<float> spawnPos = rectCenter + spawnOffset;
    swing->atkAttributes.model.SetPos(spawnPos, 4);
}

void MeleeSwingHandler::HandleCollisions(MeleeSwing *swing) {
    for (std::shared_ptr<Creatures::Creature> creature : Creatures::CreatureHandler::Instance().GetCreatureVector()) {
        std::function<bool()> shouldSkip = [swing, creature]() -> bool {
            if (!swing->atkAttributes.canHitSource && swing->atkAttributes.attackSourceID == creature->GetID() && swing->atkAttributes.canHitSourceAfter < SDL_GetTicks()) return true;

            return false;
        };
        shouldSkip();

        HandleCreatureCollision(swing, creature.get());
    }
}

void MeleeSwingHandler::HandleCreatureCollision(MeleeSwing *swing, Creatures::Creature *creature) {
    if (swing->atkAttributes.maxHits <= swing->atkAttributes.timesHit) {
        swing->atkAttributes.isMarkedForDeletion = true;
        return;
    }
    if (IsQuadColliding(swing->atkAttributes.model, Quad<float>(creature->GetAttribute().model))) {
        swing->atkAttributes.timesHit++;
        creature->Damage(swing->itemStats.damage);

        // if (!weaponInfo.canHitTheSameEntityTwice && entity.lock() != nullptr) weaponInfo.entitiesHit.push_back();  // TODO: add invulnerabilty frames to entities
    }
}

void MeleeSwingHandler::HandleLifetime(MeleeSwing *swing) {
    if (swing->atkAttributes.lifeEndTick < SDL_GetTicks()) swing->atkAttributes.isMarkedForDeletion = true;
}
}  // namespace Attacks
