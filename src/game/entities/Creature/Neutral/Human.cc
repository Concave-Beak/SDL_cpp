#include "../../../../../include/game/entities/Creature/Neutral/Human.hh"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "../../../../../include/game/entities/Creature/Components/Component.hh"
#include "../../../../../include/game/entities/Creature/Enemies/Thief.hh"
#include "../../../../../include/game/entities/Player.hh"
#include "../../../../../lib/utils/debug_utils.hh"
#include "../../../../../lib/utils/engine_utils.hh"

namespace Creatures {
Human::Human(std::unique_ptr<Components::AIComponent> aiComponent) : Creature(std::move(aiComponent)) {}
}  // namespace Creatures

namespace Creatures::Components {

void HumanBehavior::HandleBehavior(std::shared_ptr<Creatures::Creature> creature) {
    Creatures::Human* human = dynamic_cast<Creatures::Human*>(creature.get());

    human->creatureAttributes.isAggroed = false;
    if (IsRectCollidinWithTriangle(Player::GetModel(), human->sightLine) && human->creatureAttributes.isAggressiveToPlayer) {
        human->creatureAttributes.isAggroed = true;
        human->creatureAttributes.isAggroedTo = Player::GetAttributeReference();
    }
}

void HumanItemLogic::HandleItemLogic(std::shared_ptr<Creature> creature) {}

void HumanMovementPattern::HandleMovement(std::shared_ptr<Creature> creature, uint32_t directionFlag) {
    Creatures::Human* human = dynamic_cast<Creatures::Human*>(creature.get());

    Vec2<float> accelSpeed = human->entityAttributes.walkingSpeed;
    // if (isRunning) accelSpeed = playerInstance->creatureAttributes.runningSpeed;

    if (directionFlag & static_cast<uint8_t>(Direction::LEFT) && !human->entityAttributes.collisionAttributes.collidedRight) {
        human->entityAttributes.velocityNow.x -= accelSpeed.x;
        human->entityAttributes.facing = Direction::LEFT;
    }
    if (directionFlag & static_cast<uint8_t>(Direction::RIGHT) && !human->entityAttributes.collisionAttributes.collidedRight) {
        human->entityAttributes.velocityNow.x += accelSpeed.x;
        human->entityAttributes.facing = Direction::RIGHT;
    }
    if (directionFlag & static_cast<uint8_t>(Direction::UP) && !human->entityAttributes.collisionAttributes.collidedRight) {
        if (!human->entityAttributes.collisionAttributes.collidedDown) return;
        human->entityAttributes.velocityNow.y -= accelSpeed.y;
        human->entityAttributes.collisionAttributes.collidedDown = false;
    }
    if (directionFlag & static_cast<uint8_t>(Direction::DOWN)) {
        if (!human->entityAttributes.collisionAttributes.isAbovePlatform) return;
        human->entityAttributes.positionNow.y += 6;  // TODO: change how this works
        human->entityAttributes.collisionAttributes.collidedDown = false;
        human->entityAttributes.collisionAttributes.isAbovePlatform = false;
    }
}

HumanAI::HumanAI(std::unique_ptr<BehavorialComponent> behavioralComponent_, std::unique_ptr<ItemLogicComponent> itemLogicComponent_, std::unique_ptr<MovementPatternComponent> movementComponent_)
    : AIComponent(std::move(behavioralComponent_), std::move(itemLogicComponent_), std::move(movementComponent_)) {}

void HumanAI::HandleAI(std::shared_ptr<Creatures::Creature> creature) {
    Human* human = dynamic_cast<Creatures::Human*>(creature.get());
    if (!human) return;

    if (behavioralComponent) {
        behavioralComponent->HandleBehavior(creature);
    } else {
        PrintInfo(Info::CREATURE_INITIALIZED_UNPROPERLY, std::to_string(creature->GetID()));
        creature->creatureAttributes.isMarkedForDeletion = true;
    }

    if (human->creatureAttributes.isAggroed) {
        movementComponent->HandleMovement(creature, static_cast<uint8_t>(Direction::UP));
    }
}

}  // namespace Creatures::Components
