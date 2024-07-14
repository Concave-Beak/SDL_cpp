#include "../../../../include/game/entities/Creature/Creature.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <memory>

#include "../../../../include/game/entities/Creature/Neutral/Human.hh"
#include "../../../../lib/utils/sdl_utils.hh"

namespace Creatures {

std::shared_ptr<Creature> Creature::Create() {
    std::shared_ptr<Creature> creature(new Creature());
    // creatureVector.push_back(creature);
    return creature;
}

void Creature::ResetVisionCone(Creature* npc) {
    float halfAngle = npc->visionConeAngle / 2.0f;
    float angleA = float(atan2(0.0f, npc->visionConeRange)) + halfAngle;
    float angleB = float(atan2(0.0f, npc->visionConeRange)) - halfAngle;

    Vec2<float> posNpc = npc->entityAttributes.positionNow + Vec2<float>{float(npc->entityAttributes.model.w), 0};
    Vec2<float> pointA = {-(cos(angleA) * float(npc->visionConeRange)), sin(angleA) * float(npc->visionConeRange)};
    Vec2<float> pointB = {-(cos(angleB) * float(npc->visionConeRange)), sin(angleB) * float(npc->visionConeRange)};
    if (npc->entityAttributes.facing == Direction::LEFT) {
        pointA = {cos(angleA) * float(npc->visionConeRange), sin(angleA) * float(npc->visionConeRange)};
        pointB = {cos(angleB) * float(npc->visionConeRange), sin(angleB) * float(npc->visionConeRange)};
        posNpc = npc->entityAttributes.positionNow;
    }

    npc->sightLine = {
        .a = posNpc + pointA,
        .b = posNpc + pointB,
        .c = posNpc  // Head of npc
    };
}

void Creature::GenerateModelRandomness(Vec2<int*> modelWH, Vec2<int> defaultValue, int variation) {
    *modelWH.x = rand() % variation + defaultValue.x;
    *modelWH.y = rand() % variation + defaultValue.y;
    *modelWH.x *= rand() % 2;
    *modelWH.y *= rand() % 2;
}

// void Creature::Delete(std::shared_ptr<Creature> creature) {
//     creatureVector.erase(std::remove(creatureVector.begin(), creatureVector.end(), creature), creatureVector.end());
// }

//------------------------------------------------------------------------------

std::shared_ptr<CreatureHandler> CreatureHandler::Init(SDL_Renderer* renderer_) {
    handler.renderer = renderer_;
    return std::make_shared<CreatureHandler>(handler);
}

CreatureHandler& CreatureHandler::Instance() { return handler; }
CreatureHandler CreatureHandler::handler = CreatureHandler();

void CreatureHandler::PushToCreatureEntity(std::shared_ptr<Creature> creature) { creatureVector.push_back(creature); }

void CreatureHandler::Handle(Vec2<float> playerPos, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused) {
    for (CreatureVector::iterator it = creatureVector.begin(); it != creatureVector.end(); ++it) {
        HandleVelocity((*it).get(), timeDelta, timeMultiplier, isPaused);
        HandleCollisions(**it, timeDelta, timeMultiplier, isPaused);
        Draw(**it, cameraPos);
    }
}

bool CreatureHandler::GetCollidedInformation(const Creature& creature, Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return creature.collisionAttributes.collidedUp;
        }
        case Direction::LEFT: {
            return creature.collisionAttributes.collidedUp;
        }
        case Direction::RIGHT: {
            return creature.collisionAttributes.collidedUp;
        }
        case Direction::DOWN: {
            return creature.collisionAttributes.collidedUp;
        }
    }
    return false;
}

void CreatureHandler::HandleVelocity(Creature* creature, float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    if (creature->entityAttributes.velocityNow.y > MAX_Y_SPEED) {
        creature->entityAttributes.velocityNow.y = MAX_Y_SPEED;
    }
    if (creature->entityAttributes.velocityNow.y < -MAX_Y_SPEED) {
        creature->entityAttributes.velocityNow.y = -MAX_Y_SPEED;
    }

    if (creature->entityAttributes.velocityNow.x > MAX_X_SPEED) {
        creature->entityAttributes.velocityNow.x = MAX_X_SPEED;
    }
    if (creature->entityAttributes.velocityNow.x < -MAX_X_SPEED) {
        creature->entityAttributes.velocityNow.x = -MAX_X_SPEED;
    }

    if (!creature->collisionAttributes.collidedDown) {
        creature->entityAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        creature->entityAttributes.positionNow.y += creature->entityAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!creature->collisionAttributes.collidedDown) creature->collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    creature->entityAttributes.velocityNow.x -= timeDelta * creature->collisionAttributes.surfaceAttrition * timeMultiplier * creature->entityAttributes.velocityNow.x;
    creature->entityAttributes.positionNow.x += creature->entityAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void CreatureHandler::ResetCollisionState(Creature& creature) {
    creature.collisionAttributes.collidedDown = false, creature.collisionAttributes.collidedUp = false,
    creature.collisionAttributes.collidedLeft = false, creature.collisionAttributes.collidedRight = false;
    creature.collisionAttributes.isAbovePlatform = false;
}

void CreatureHandler::HandleVerticalCollision(Creature& creature, const LevelItem& levelItem,
                                              float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(creature.entityAttributes.model, levelItem.rect, Direction::DOWN, creature.entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        creature.collisionAttributes.collidedDown = true;
        creature.collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        creature.entityAttributes.positionNow.y = levelItemTop - float(creature.entityAttributes.model.h);
        creature.entityAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) creature.collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(creature.entityAttributes.model, levelItem.rect, Direction::UP, creature.entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        creature.collisionAttributes.collidedUp = true;
        creature.entityAttributes.positionNow.y = levelItemBottom;
        creature.entityAttributes.velocityNow.y = -creature.entityAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void CreatureHandler::HandleHorizontalCollision(Creature& creature, const LevelItem& levelItem,
                                                float timeDelta, float timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(creature.entityAttributes.model, levelItem.rect, Direction::RIGHT, creature.entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        creature.collisionAttributes.collidedRight = true;
        creature.entityAttributes.velocityNow.x = -creature.entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        creature.entityAttributes.positionNow.x = levelItemLeft - float(creature.entityAttributes.model.w);
    }

    bool hitLeft = IsSideColliding(creature.entityAttributes.model, levelItem.rect, Direction::LEFT, creature.entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        creature.collisionAttributes.collidedLeft = true;
        creature.entityAttributes.velocityNow.x = -creature.entityAttributes.velocityNow.x * SURFACE_BOUNCE;
        creature.entityAttributes.positionNow.x = levelItemRight;
    }
}

void CreatureHandler::HandleCollisions(Creature& creature, float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    ResetCollisionState(creature);

    for (LevelItem levelItem : Level::collisions) {
        UpdateModel(creature);
        HandleVerticalCollision(creature, levelItem, timeDelta, timeMultiplier);  // TODO remove model
        UpdateModel(creature);
        HandleHorizontalCollision(creature, levelItem, timeDelta, timeMultiplier);
    }
}

void CreatureHandler::Draw(const Creature& creature, Vec2<int> cameraPos) {
    Vec2<float> positionNow = creature.entityAttributes.positionNow;
    SDL_Rect playerModel = {
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        creature.entityAttributes.model.w,
        creature.entityAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
}
void CreatureHandler::DrawSightLine(Creature& creature, Vec2<int> cameraPos, SDL_Renderer* renderer) {}

void CreatureHandler::UpdateModel(Creature& creature) {
    creature.entityAttributes.model = SDL_Rect{
        .x = int(creature.entityAttributes.positionNow.x),
        .y = int(creature.entityAttributes.positionNow.y),
        .w = creature.entityAttributes.model.w,
        .h = creature.entityAttributes.model.h,
    };
}

}  // namespace Creatures
