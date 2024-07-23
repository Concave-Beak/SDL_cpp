#include "../../../../../include/game/entities/Creature/Components/Component.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include <functional>

#include "../../../../../include/game/entities/Creature/Creature.hh"
#include "../../../../../include/main/Level.hh"
#include "../../../../../lib/utils/sdl_utils.hh"

namespace Creatures::Components {

AIComponent::AIComponent(std::unique_ptr<BehavorialComponent> behavioralComponent_,
                         std::unique_ptr<ItemLogicComponent> itemLogicComponent_,
                         std::unique_ptr<MovementPatternComponent> movementComponent_)
    : behavioralComponent(std::move(behavioralComponent_)),
      itemLogicComponent(std::move(itemLogicComponent_)),
      movementComponent(std::move(movementComponent_)) {}

void DrawingComponent::Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, std::shared_ptr<Creatures::Creature> creature) {
    SDL_Rect modelToDraw = creature->entityAttributes.model;
    modelToDraw.x -= cameraPos.x;
    modelToDraw.y -= cameraPos.y;

    scc(SDL_SetRenderDrawColor(renderer, RED, 0xff)).Handle();
    scc(SDL_RenderFillRect(renderer, &modelToDraw)).Handle();
}

void PhysicsComponent::HandlePhysics(std::shared_ptr<Creatures::Creature> creature, float timeDelta, float timeMultiplier) {
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

    if (!creature->entityAttributes.collisionAttributes.collidedDown) {
        creature->entityAttributes.velocityNow.y += timeDelta * GRAVITY * timeMultiplier;
        creature->entityAttributes.positionNow.y += creature->entityAttributes.velocityNow.y * timeDelta * timeMultiplier;
    }

    if (!creature->entityAttributes.collisionAttributes.collidedDown) creature->entityAttributes.collisionAttributes.surfaceAttrition = AIR_ATTRITION;

    creature->entityAttributes.velocityNow.x -= timeDelta * creature->entityAttributes.collisionAttributes.surfaceAttrition * timeMultiplier * creature->entityAttributes.velocityNow.x;
    creature->entityAttributes.positionNow.x += creature->entityAttributes.velocityNow.x * timeMultiplier * timeDelta;
}

void CollisionComponent::HandleCollision(std::shared_ptr<Creatures::Creature> creature, float timeDelta, float timeMultiplier) {
    creature->ResetCollisionState();

    for (LevelItem levelItem : Level::collisions) {
        creature->UpdateModel();
        HandleVerticalCollision(creature, creature->entityAttributes.model, levelItem, timeDelta, timeMultiplier);
    }
}

void CollisionComponent::HandleVerticalCollision(std::shared_ptr<Creatures::Creature> creature, SDL_Rect entityRect, const LevelItem& levelItem, float timeDelta, float timeMultiplier) {
    float levelItemTop = float(levelItem.pos.y),
          levelItemBottom = float(levelItem.pos.y + levelItem.rect.h);

    bool hitFeet = IsSideColliding(entityRect, levelItem.rect, Direction::DOWN, creature->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitFeet) {
        creature->entityAttributes.collisionAttributes.collidedDown = true;
        creature->entityAttributes.collisionAttributes.surfaceAttrition = levelItem.attritionCoefficient;
        creature->entityAttributes.positionNow.y = levelItemTop - float(creature->entityAttributes.model.h);
        creature->entityAttributes.velocityNow.y = 0;
        if (levelItem.collisionType == PLATFORM) creature->entityAttributes.collisionAttributes.isAbovePlatform = true;
    }

    if (levelItem.collisionType == CollisionType::PLATFORM) return;

    bool hitHead = IsSideColliding(entityRect, levelItem.rect, Direction::UP, creature->entityAttributes.velocityNow, timeDelta, timeMultiplier);
    if (hitHead) {
        creature->entityAttributes.collisionAttributes.collidedUp = true;
        creature->entityAttributes.positionNow.y = levelItemBottom;
        creature->entityAttributes.velocityNow.y = -creature->entityAttributes.velocityNow.y * SURFACE_BOUNCE;
    }
}

void CollisionComponent::HandleHorizontalCollision(std::shared_ptr<Creatures::Creature> creature, SDL_Rect entityRect, const LevelItem& levelItem, float timeDelta, float timeMultiplier) {}

}  // namespace Creatures::Components
