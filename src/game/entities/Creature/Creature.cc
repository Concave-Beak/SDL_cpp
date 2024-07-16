#include "../../../../include/game/entities/Creature/Creature.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>

#include "../../../../include/game/entities/Creature/Neutral/Human.hh"
#include "../../../../lib/utils/sdl_utils.hh"

namespace Creatures {

std::shared_ptr<Creature> Creature::Create() {
    std::shared_ptr<Creature> creature(new Creature());
    // creatureVector.push_back(creature);
    return creature;
}

Uint32 Creature::GetID() { return creatureAttributes.GetID(); }

CreatureAttributes Creature::GetAttribute() { return creatureAttributes; }
const CreatureAttributes* Creature::GetAttributeReference() { return &creatureAttributes; }

void Creature::Damage(int damage) {
    if (creatureAttributes.combatAttributes.heathNow > 0) {
        creatureAttributes.combatAttributes.heathNow -= damage;
    }

    if (creatureAttributes.combatAttributes.heathNow <= 0) {
        creatureAttributes.combatAttributes.isDead = true;
    }
}

void Creature::GenerateModelRandomness(Vec2<int*> modelWH, Vec2<int> defaultValue, int variation) {
    int randomVariationX = (rand() % (2 * variation + 1)) - variation;
    *modelWH.x = defaultValue.x + randomVariationX;
    int randomVariationY = (rand() % (2 * variation + 1)) - variation;
    *modelWH.y = defaultValue.y + randomVariationY;
}

//------------------------------------------------------------------------------

std::shared_ptr<CreatureHandler> CreatureHandler::Init(SDL_Renderer* renderer_) {
    handler.renderer = renderer_;
    return std::make_shared<CreatureHandler>(handler);
}

CreatureHandler& CreatureHandler::Instance() { return handler; }
CreatureHandler CreatureHandler::handler = CreatureHandler();

const std::vector<std::shared_ptr<Creature>>& CreatureHandler::GetCreatureVector() { return creatureVector; }

void CreatureHandler::PushToCreatureVector(std::shared_ptr<Creature> creature) { creatureVector.push_back(creature); }

void CreatureHandler::Delete(std::shared_ptr<Creature> creature) {
    creatureVector.erase(std::remove(creatureVector.begin(), creatureVector.end(), creature), creatureVector.end());
}

void CreatureHandler::Handle(Vec2<float> playerPos, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused) {
    for (CreatureVector::iterator it = creatureVector.begin(); it != creatureVector.end();) {
        if ((*it)->isMarkedForDeletion) {
            Delete(*it);
            continue;
        }
        HandleVelocity(*(*it).get(), timeDelta, timeMultiplier, isPaused);
        HandleCollisions(**it, timeDelta, timeMultiplier, isPaused);
        ResetVisionCone(**it);
        DrawSightLine(**it, cameraPos);
        Draw(**it, cameraPos);
        ++it;
    }
}

bool CreatureHandler::GetCollidedInformation(const Creature& creature, Direction direction) {
    switch (direction) {
        case Direction::UP: {
            return creature.creatureAttributes.collisionAttributes.collidedUp;
        }
        case Direction::LEFT: {
            return creature.creatureAttributes.collisionAttributes.collidedUp;
        }
        case Direction::RIGHT: {
            return creature.creatureAttributes.collisionAttributes.collidedUp;
        }
        case Direction::DOWN: {
            return creature.creatureAttributes.collisionAttributes.collidedUp;
        }
    }
    return false;
}

void CreatureHandler::HandleVelocity(Creature& creature, float timeDelta, float timeMultiplier, bool isPaused) {
    if (isPaused) return;

    if (creature.creatureAttributes.velocityNow.y > MAX_Y_SPEED) {
        creature.creatureAttributes.velocityNow.y = MAX_Y_SPEED;
    }
    if (creature.creatureAttributes.velocityNow.y < -MAX_Y_SPEED) {
        creature.creatureAttributes.velocityNow.y = -MAX_Y_SPEED;
    }

    if (creature.creatureAttributes.velocityNow.x > MAX_X_SPEED) {
        creature.creatureAttributes.velocityNow.x = MAX_X_SPEED;
    }
    if (creature.creatureAttributes.velocityNow.x < -MAX_X_SPEED) {
        creature.creatureAttributes.velocityNow.x = -MAX_X_SPEED;
    }

    if (!creature.creatureAttributes.collisionAttributes.collidedDown) {
        creature.creatureAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        creature.creatureAttributes.positionNow.y += creature.creatureAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!creature.creatureAttributes.collisionAttributes.collidedDown) creature.creatureAttributes.collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    creature.creatureAttributes.velocityNow.x -= timeDelta * creature.creatureAttributes.collisionAttributes.surfaceAttrition * timeMultiplier * creature.creatureAttributes.velocityNow.x;
    creature.creatureAttributes.positionNow.x += creature.creatureAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void CreatureHandler::ResetCollisionState(Creature& creature) {
    creature.creatureAttributes.collisionAttributes.collidedDown = false, creature.creatureAttributes.collisionAttributes.collidedUp = false,
    creature.creatureAttributes.collisionAttributes.collidedLeft = false, creature.creatureAttributes.collisionAttributes.collidedRight = false;
    creature.creatureAttributes.collisionAttributes.isAbovePlatform = false;
}

void CreatureHandler::HandleVerticalCollision(Creature& creature, const LevelItem& levelItem,
                                              float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(creature.creatureAttributes.model, levelItem.rect, Direction::DOWN, creature.creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        creature.creatureAttributes.collisionAttributes.collidedDown = true;
        creature.creatureAttributes.collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        creature.creatureAttributes.positionNow.y = levelItemTop - float(creature.creatureAttributes.model.h);
        creature.creatureAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) creature.creatureAttributes.collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(creature.creatureAttributes.model, levelItem.rect, Direction::UP, creature.creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        creature.creatureAttributes.collisionAttributes.collidedUp = true;
        creature.creatureAttributes.positionNow.y = levelItemBottom;
        creature.creatureAttributes.velocityNow.y = -creature.creatureAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void CreatureHandler::HandleHorizontalCollision(Creature& creature, const LevelItem& levelItem,
                                                float timeDelta, float timeMultiplier) {
    if (levelItem.collisionType == PLATFORM) return;

    float levelItemLeft = float(levelItem.pos.x),
          levelItemRight = float(levelItem.pos.x + levelItem.rect.w);

    bool hitRight = IsSideColliding(creature.creatureAttributes.model, levelItem.rect, Direction::RIGHT, creature.creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitRight) {
        creature.creatureAttributes.collisionAttributes.collidedRight = true;
        creature.creatureAttributes.velocityNow.x = -creature.creatureAttributes.velocityNow.x * SURFACE_BOUNCE;
        creature.creatureAttributes.positionNow.x = levelItemLeft - float(creature.creatureAttributes.model.w);
    }

    bool hitLeft = IsSideColliding(creature.creatureAttributes.model, levelItem.rect, Direction::LEFT, creature.creatureAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitLeft) {
        creature.creatureAttributes.collisionAttributes.collidedLeft = true;
        creature.creatureAttributes.velocityNow.x = -creature.creatureAttributes.velocityNow.x * SURFACE_BOUNCE;
        creature.creatureAttributes.positionNow.x = levelItemRight;
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
    Vec2<float> positionNow = creature.creatureAttributes.positionNow;
    SDL_Rect playerModel = {
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        creature.creatureAttributes.model.w,
        creature.creatureAttributes.model.h,
    };
    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &playerModel)).Handle();
}

void CreatureHandler::DrawSightLine(Creature& creature, Vec2<int> cameraPos) {
    creature.sightLine.Draw(renderer, cameraPos, {BLACK, 0x88});
}

void CreatureHandler::UpdateModel(Creature& creature) {
    creature.creatureAttributes.model = SDL_Rect{
        .x = int(creature.creatureAttributes.positionNow.x),
        .y = int(creature.creatureAttributes.positionNow.y),
        .w = creature.creatureAttributes.model.w,
        .h = creature.creatureAttributes.model.h,
    };
}

void CreatureHandler::ResetVisionCone(Creature& npc) {
    float halfAngle = npc.visionConeAngle / 2.0f;
    float angleA = float(atan2(0.0f, npc.visionConeRange)) + halfAngle;
    float angleB = float(atan2(0.0f, npc.visionConeRange)) - halfAngle;

    Vec2<float> posNpc = npc.creatureAttributes.positionNow + Vec2<float>{float(npc.creatureAttributes.model.w), 0};
    Vec2<float> pointA = {-(cos(angleA) * float(npc.visionConeRange)), sin(angleA) * float(npc.visionConeRange)};
    Vec2<float> pointB = {-(cos(angleB) * float(npc.visionConeRange)), sin(angleB) * float(npc.visionConeRange)};
    if (npc.creatureAttributes.facing == Direction::LEFT) {
        pointA = {cos(angleA) * float(npc.visionConeRange), sin(angleA) * float(npc.visionConeRange)};
        pointB = {cos(angleB) * float(npc.visionConeRange), sin(angleB) * float(npc.visionConeRange)};
        posNpc = npc.creatureAttributes.positionNow;
    }

    npc.sightLine = {
        .a = posNpc + pointA,
        .b = posNpc + pointB,
        .c = posNpc  // Head of npc
    };
}

}  // namespace Creatures
