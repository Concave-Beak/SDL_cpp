#include "../../../../include/game/entities/Creature/Creature.hh"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <ostream>
#include <string>

#include "../../../../lib/utils/sdl_utils.hh"

namespace Creatures {

void GenerateModelRandomness(Vec2<int*> modelWH, Vec2<int> defaultValue, int variation) {
    int randomVariationX = (rand() % (2 * variation + 1)) - variation;
    *modelWH.x = defaultValue.x + randomVariationX;
    int randomVariationY = (rand() % (2 * variation + 1)) - variation;
    *modelWH.y = defaultValue.y + randomVariationY;
}

//------------------------------------------------------------------------------

Creature::Creature(std::unique_ptr<Components::AIComponent> aiComponent_) : aiComponent(std::move(aiComponent_)) {
    drawingComponent = std::make_unique<Components::DrawingComponent>();
    physicsComponent = std::make_unique<Components::PhysicsComponent>();
    collisionComponent = std::make_unique<Components::CollisionComponent>();
}

Uint32 Creature::GetID() { return entityAttributes.GetID(); }

EntityAttributes Creature::GetAttributeCopy() { return EntityAttributes(entityAttributes); }
const EntityAttributes* Creature::GetAttributeConstReference() { return &entityAttributes; }

void Creature::Damage(int damage) {
    if (entityAttributes.combatAttributes.heathNow > 0) {
        entityAttributes.combatAttributes.heathNow -= damage;
    }

    if (entityAttributes.combatAttributes.heathNow <= 0) {
        entityAttributes.combatAttributes.isDead = true;
    }
}

//------------------------------------------------------------------------------

std::vector<std::shared_ptr<Creature>> CreatureHandler::creatureVector = {};
const std::vector<std::shared_ptr<Creature>>& CreatureHandler::GetCreatureVector() { return creatureVector; }

void CreatureHandler::PushToCreatureVector(std::shared_ptr<Creature> creature) { creatureVector.push_back(creature); }

void CreatureHandler::Delete(std::shared_ptr<Creature> creature) {
    creatureVector.erase(std::remove(creatureVector.begin(), creatureVector.end(), creature), creatureVector.end());
}

void CreatureHandler::InvokeHandler(Vec2<int> cameraPos, SDL_Renderer* renderer, float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;
    CheckDeletion();
    for (std::shared_ptr<Creature> creature : CreatureHandler::GetCreatureVector()) {
        DrawSightLine(creature, cameraPos, renderer);  // Should be displayed only when debug_mode is on
        ResetVisionCone(creature);

        creature->aiComponent->HandleAI(creature);
        creature->drawingComponent->Draw(renderer, cameraPos, creature);
        creature->physicsComponent->HandlePhysics(creature, timeDelta, timeMultiplier);
        creature->collisionComponent->HandleCollision(creature, timeDelta, timeMultiplier);
    }
}

void CreatureHandler::CheckDeletion() {
    for (CreatureVector::iterator it = creatureVector.begin(); it != creatureVector.end();) {
        if ((*it)->isMarkedForDeletion) {
            Delete(*it);
            continue;
        }
        ++it;
    }
}

bool Creature::GetCollidedInformation(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return entityAttributes.collisionAttributes.collidedUp;
        }
        case Direction::LEFT: {
            return entityAttributes.collisionAttributes.collidedUp;
        }
        case Direction::RIGHT: {
            return entityAttributes.collisionAttributes.collidedUp;
        }
        case Direction::DOWN: {
            return entityAttributes.collisionAttributes.collidedUp;
        }
    }
    return false;
}

void Creature::ResetCollisionState() {
    entityAttributes.collisionAttributes.collidedDown = false, entityAttributes.collisionAttributes.collidedUp = false,
    entityAttributes.collisionAttributes.collidedLeft = false, entityAttributes.collisionAttributes.collidedRight = false;
    entityAttributes.collisionAttributes.isAbovePlatform = false;
}

void Creature::UpdateModel() {
    entityAttributes.model = SDL_Rect{
        .x = int(entityAttributes.positionNow.x),
        .y = int(entityAttributes.positionNow.y),
        .w = entityAttributes.model.w,
        .h = entityAttributes.model.h,
    };
}

// TODO: remove
void CreatureHandler::DrawSightLine(const std::shared_ptr<Creature> creature, Vec2<int> cameraPos, SDL_Renderer* renderer) {
    creature->sightLine.Draw(renderer, cameraPos, {BLACK, 0x88});
}

void CreatureHandler::ResetVisionCone(std::shared_ptr<Creature> creature) {
    float halfAngle = creature->visionConeAngle / 2.0f;
    float angleA = float(atan2(0.0f, creature->visionConeRange)) + halfAngle;
    float angleB = float(atan2(0.0f, creature->visionConeRange)) - halfAngle;

    Vec2<float> posNpc;
    Vec2<float> pointA;
    Vec2<float> pointB;

    if (creature->entityAttributes.facing == Direction::LEFT) {
        pointA = {cos(angleA) * float(creature->visionConeRange), sin(angleA) * float(creature->visionConeRange)};
        pointB = {cos(angleB) * float(creature->visionConeRange), sin(angleB) * float(creature->visionConeRange)};
        posNpc = creature->entityAttributes.positionNow;
    } else {
        posNpc = creature->entityAttributes.positionNow + Vec2<float>{float(creature->entityAttributes.model.w), 0};
        pointA = {-(cos(angleA) * float(creature->visionConeRange)), sin(angleA) * float(creature->visionConeRange)};
        pointB = {-(cos(angleB) * float(creature->visionConeRange)), sin(angleB) * float(creature->visionConeRange)};
    }

    creature->sightLine = {
        .a = posNpc + pointA,
        .b = posNpc + pointB,
        .c = posNpc  // Head of npc
    };
}

//------------------------------------------------------------------------------

/*
void HumanoidHandler::Handler(std::shared_ptr<Creature> creature, Vec2<float> cameraPos, float timeDelta, float timeMultiplier, SDL_Renderer* renderer) {
    Humanoid* humanoid = dynamic_cast<Humanoid*>(creature.get());
    instanceHandler.HandleAggro(humanoid);
    instanceHandler.HandleVelocity(humanoid, timeDelta, timeMultiplier);
    instanceHandler.HandleCollisions(humanoid, timeDelta, timeMultiplier);
    instanceHandler.Draw(humanoid, cameraPos, renderer);
}

HumanoidHandler HumanoidHandler::instanceHandler = HumanoidHandler();

void HumanoidHandler::HandleVelocity(Humanoid* humanoid, float timeDelta, float timeMultiplier) {
    if (humanoid->entityAttributes.velocityNow.y > MAX_Y_SPEED) {
        humanoid->entityAttributes.velocityNow.y = MAX_Y_SPEED;
    }
    if (humanoid->entityAttributes.velocityNow.y < -MAX_Y_SPEED) {
        humanoid->entityAttributes.velocityNow.y = -MAX_Y_SPEED;
    }

    if (humanoid->entityAttributes.velocityNow.x > MAX_X_SPEED) {
        humanoid->entityAttributes.velocityNow.x = MAX_X_SPEED;
    }
    if (humanoid->entityAttributes.velocityNow.x < -MAX_X_SPEED) {
        humanoid->entityAttributes.velocityNow.x = -MAX_X_SPEED;
    }

    if (!humanoid->entityAttributes.collisionAttributes.collidedDown) {
        humanoid->entityAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        humanoid->entityAttributes.positionNow.y += humanoid->entityAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!humanoid->entityAttributes.collisionAttributes.collidedDown) humanoid->entityAttributes.collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    humanoid->entityAttributes.velocityNow.x -= timeDelta * humanoid->entityAttributes.collisionAttributes.surfaceAttrition * timeMultiplier * humanoid->entityAttributes.velocityNow.x;
    humanoid->entityAttributes.positionNow.x += humanoid->entityAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void HumanoidHandler::HandleVerticalCollision(Humanoid* humanoid, const LevelItem& levelItem, float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y), levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(humanoid->entityAttributes.model, levelItem.rect, Direction::DOWN, humanoid->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        humanoid->entityAttributes.collisionAttributes.collidedDown = true;
        humanoid->entityAttributes.collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        humanoid->entityAttributes.positionNow.y = levelItemTop - float(humanoid->entityAttributes.model.h);
        humanoid->entityAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) humanoid->entityAttributes.collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(humanoid->entityAttributes.model, levelItem.rect, Direction::UP, humanoid->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        humanoid->entityAttributes.collisionAttributes.collidedUp = true;
        humanoid->entityAttributes.positionNow.y = levelItemBottom;
        humanoid->entityAttributes.velocityNow.y = -humanoid->entityAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void HumanoidHandler::HandleHorizontalCollision(Humanoid* humanoid, const LevelItem& levelItem, float timeDelta, float timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(humanoid->entityAttributes.model, levelItem.rect, Direction::RIGHT, humanoid->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        humanoid->entityAttributes.collisionAttributes.collidedRight = true;
        humanoid->entityAttributes.velocityNow.x = -humanoid->entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        humanoid->entityAttributes.positionNow.x = levelItemLeft - float(humanoid->entityAttributes.model.w);
    }

    bool hitLeft = IsSideColliding(humanoid->entityAttributes.model, levelItem.rect, Direction::LEFT, humanoid->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        humanoid->entityAttributes.collisionAttributes.collidedLeft = true;
        humanoid->entityAttributes.velocityNow.x = -humanoid->entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        humanoid->entityAttributes.positionNow.x = levelItemRight;
    }
}

void HumanoidHandler::HandleCollisions(Humanoid* humanoid, float timeDelta, float timeMultiplier) {
    humanoid->ResetCollisionState();

    for (LevelItem levelItem : Level::collisions) {
        humanoid->UpdateModel();
        HandleVerticalCollision(humanoid, levelItem, timeDelta, timeMultiplier);  // TODO remove model
        humanoid->UpdateModel();
        HandleHorizontalCollision(humanoid, levelItem, timeDelta, timeMultiplier);
    }
}

void HumanoidHandler::HandleAggro(Humanoid* humanoid) {
    if (humanoid->creatureAttributes.aggressionTypeAgainstPlayer == CreatureAttributes::AggressionType::PASSIVE) return;

    SDL_Rect playerModel = Player::GetModel();
    Vec2<float> middlePlayer = {
        static_cast<float>(playerModel.x + playerModel.w * 0.5f),
        static_cast<float>(playerModel.y + playerModel.h * 0.5f),
    };
    if (IsPointInTriangle(middlePlayer, humanoid->sightLine)) {
        humanoid->creatureAttributes.lastSeenAttackerPos = middlePlayer;
        humanoid->creatureAttributes.isAggroed = true;
    }
    if (humanoid->creatureAttributes.isAggroed) {
        Move(humanoid);
    }
}

void HumanoidHandler::Move(Humanoid* humanoid) {
    if (humanoid->entityAttributes.positionNow.x > humanoid->creatureAttributes.lastSeenAttackerPos.x) {
        humanoid->entityAttributes.velocityNow += humanoid->entityAttributes.walkingSpeed;
    } else {
        humanoid->entityAttributes.velocityNow -= humanoid->entityAttributes.walkingSpeed;
    }
}

void HumanoidHandler::Draw(const Humanoid* humanoid, Vec2<int> cameraPos, SDL_Renderer* renderer) {
    Vec2<float> positionNow = humanoid->entityAttributes.positionNow;
    SDL_Rect playerModel = {
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        humanoid->entityAttributes.model.w,
        humanoid->entityAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
}

*/
}  // namespace Creatures
