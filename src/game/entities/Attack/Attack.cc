#include "../../../../include/game/entities/Attack/Attack.hh"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <csignal>
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

void Arrow::Init(Items::ItemStats itemStats_, EntityAttributes *entityAttributes, float angle_, Vec2<int> positionNow_, int dimentions, Vec2<int> velocity) {
    itemStats = itemStats_;
    atkAttributes.attackType = AttackType::ARROW_PROJECTILE;
    positionNow = positionNow_;
    atkAttributes.isEffectedByGravity = true;
    atkAttributes.model = {positionNow_.x, positionNow_.y, dimentions, dimentions};
    atkAttributes.angle = angle_;
    maximumVelocity = velocity;
    velocityNow = {
        400 * cos(angle_) * itemStats.chargeNow / 100,
        400 * sin(angle_) * itemStats.chargeNow / 100,
    };

    atkAttributes.attackSourceID = entityAttributes->GetID();

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
}

void ArrowHandler::Draw(const Arrow &arrow, Vec2<int> cameraPos, SDL_Renderer *renderer) {
    Vec2<int> posNow = {arrow.atkAttributes.model.x, arrow.atkAttributes.model.y};
    SDL_Rect arrowModel = {
        posNow.x - cameraPos.x,
        posNow.y - cameraPos.y,
        arrow.atkAttributes.model.w,
        arrow.atkAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &arrowModel)).Handle();
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

    arrow->atkAttributes.model.x = arrow->positionNow.x;
    arrow->atkAttributes.model.y = arrow->positionNow.y;
}

void ArrowHandler::HandleCollisions(Arrow *arrow) {
    if (arrow->isStuckToSurface || arrow->isStuckToEntity) {
        return;
    }

    HandlePlayerCollision(arrow);

    for (std::shared_ptr<Creatures::Creature> creature : Creatures::CreatureHandler::GetCreatureVector()) {
        std::function<bool()> shouldSkip = [arrow, creature]() -> bool {
            if (!arrow->atkAttributes.canHitSource && arrow->atkAttributes.attackSourceID == creature->GetID()) return true;

            if (arrow->atkAttributes.canHitSourceAfter > SDL_GetTicks()) return true;

            return false;
        };
        if (shouldSkip()) continue;

        HandleCreatureCollision(arrow, creature.get());
    }

    for (LevelItem levelItem : Level::collisions) {
        HandleSurfaceCollision(arrow, levelItem.rect);
    }
}

void ArrowHandler::HandleStuck(Arrow *arrow) {
    if (arrow->isStuckToEntity && arrow->stuckEntity == nullptr) {
        arrow->isStuckToEntity = false;
        return;
    }
    if (arrow->isStuckToEntity) {
        arrow->atkAttributes.model.x = arrow->posStuck.x + arrow->stuckEntity->positionNow.x;
        arrow->atkAttributes.model.y = arrow->posStuck.y + arrow->stuckEntity->positionNow.y;
        return;
    }
    if (arrow->isStuckToSurface) {
        arrow->atkAttributes.model.x = arrow->posStuck.x;
        arrow->atkAttributes.model.y = arrow->posStuck.y;
        return;
    }
}

void ArrowHandler::HandleCreatureCollision(Arrow *arrow, Creatures::Creature *creature) {
    if (IsRectangleCollidingWithRect(arrow->atkAttributes.model, (creature->GetAttributeCopy().model))) {
        arrow->velocityNow = {0, 0};
        arrow->isStuckToEntity = true;
        arrow->stuckEntity = creature->GetAttributeConstReference();
        arrow->posStuck = arrow->positionNow - creature->GetAttributeCopy().positionNow;
        creature->Damage(arrow->itemStats.damage);
    }
}

void ArrowHandler::HandleSurfaceCollision(Arrow *arrow, const SDL_Rect &surfaceRect) {
    if (IsRectangleCollidingWithRect(arrow->atkAttributes.model, surfaceRect)) {
        arrow->isStuckToSurface = true;
        arrow->posStuck = arrow->positionNow;
        arrow->velocityNow = {0, 0};
    }
}

void ArrowHandler::HandlePlayerCollision(Arrow *arrow) {
    // Check to see if can hit source and player is source and if can hit now
    if (!arrow->atkAttributes.canHitSource && Player::GetAttribute().GetID() == arrow->atkAttributes.attackSourceID) return;
    if (arrow->atkAttributes.canHitSourceAfter > SDL_GetTicks()) return;

    if (IsRectangleCollidingWithRect(arrow->atkAttributes.model, Player::GetAttribute().model)) {
        arrow->isStuckToEntity = true;
        arrow->stuckEntity = Player::GetAttributeReference();
        arrow->posStuck = arrow->positionNow - Player::GetAttribute().positionNow;
    }
}

//------------------------------------------------------------------------------

void MeleeSwing::Init(Items::ItemStats itemStats_, EntityAttributes *entityOrigin_, float angle_, Vec2<int> positionNow_, int minDimention_, int maxDimention_) {
    atkAttributes.attackType = AttackType::SWORD_SLASH;
    itemStats = Items::ItemStats(itemStats_);
    atkAttributes.model = {positionNow_.x, positionNow_.y, minDimention, minDimention};
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

    atkAttributes.model.x = spawnPos.x;
    atkAttributes.model.y = spawnPos.y;
    minDimention = minDimention_;
    maxDimention = maxDimention_;
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
    Vec2<int> posNow = {swing->atkAttributes.model.x, swing->atkAttributes.model.y};
    SDL_Rect swingModel = {
        posNow.x - cameraPos.x,
        posNow.y - cameraPos.y,
        swing->atkAttributes.model.w,
        swing->atkAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, BLACK, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &swingModel)).Handle();
}

void MeleeSwingHandler::UpdateModel(MeleeSwing *swing) {
    if (swing->atkAttributes.isMarkedForDeletion) return;

    Vec2<float> rectCenter = Vec2<float>{
        swing->creatureOrigin->model.x + swing->creatureOrigin->model.w / 2.0f,
        swing->creatureOrigin->model.y + swing->creatureOrigin->model.h / 2.0f,
    };

    float radius = sqrt(pow(swing->creatureOrigin->model.w * 0.6f, 2) + pow(swing->creatureOrigin->model.h * 0.6f, 2));
    Vec2<float> offset = Vec2<float>{
        radius * cos(swing->atkAttributes.angle),
        radius * sin(swing->atkAttributes.angle),
    };

    Vec2<float> newPos = rectCenter + offset;

    // TODO
    swing->atkAttributes.model.x = newPos.x;
    swing->atkAttributes.model.y = newPos.y;
    swing->atkAttributes.model.w = swing->minDimention;
    swing->atkAttributes.model.h = swing->minDimention;
    if (float cosOfAngle = cos(swing->atkAttributes.angle) > swing->minDimention) {
        swing->atkAttributes.model.w = cos(cosOfAngle) * swing->maxDimention;
    }
    if (float cosOfAngle = sin(swing->atkAttributes.angle) > swing->minDimention) {
        swing->atkAttributes.model.w = sin(cosOfAngle) * swing->maxDimention;
    }
}

void MeleeSwingHandler::HandleCollisions(MeleeSwing *swing) {
    for (std::shared_ptr<Creatures::Creature> creature : Creatures::CreatureHandler::GetCreatureVector()) {
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
    if (IsRectangleCollidingWithRect(swing->atkAttributes.model, creature->GetAttributeCopy().model)) {
        swing->atkAttributes.timesHit++;
        creature->Damage(swing->itemStats.damage);

        // if (!weaponInfo.canHitTheSameEntityTwice && entity.lock() != nullptr) weaponInfo.entitiesHit.push_back();  // TODO: add invulnerabilty frames to entities
    }
}

void MeleeSwingHandler::HandleLifetime(MeleeSwing *swing) {
    if (swing->atkAttributes.lifeEndTick < SDL_GetTicks()) swing->atkAttributes.isMarkedForDeletion = true;
}
}  // namespace Attacks
