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
    if (!human) return;
    if (human->creatureAttributes.isAggroed) return;
}

void HumanItemLogic::HandleItemLogic(std::shared_ptr<Creature> creature) {}

void HumanMovement::HandleMovement(std::shared_ptr<Creature> creature) {
    Creatures::Human* human = dynamic_cast<Creatures::Human*>(creature.get());
    if (!human) return;
    if (!human->creatureAttributes.isAggroed && !human->creatureAttributes.isWandering) return;

    uint8_t directionFlag;
    if (human->entityAttributes.positionNow.x > human->creatureAttributes.goingToPos.x) {
        directionFlag = static_cast<uint8_t>(Direction::LEFT);
    }
    if (human->entityAttributes.positionNow.x < human->creatureAttributes.goingToPos.x) {
        directionFlag = static_cast<uint8_t>(Direction::RIGHT);
    }

    auto moveToDirection = [human](uint8_t directionFlag) {
        Vec2<float> accelSpeed = human->entityAttributes.walkingSpeed;
        if (human->entityAttributes.isRunning) accelSpeed = human->entityAttributes.runningSpeed;
        if (human->entityAttributes.isSneaking) accelSpeed = human->entityAttributes.sneakingSpeed;

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
    };
    moveToDirection(directionFlag);
}

void HumanAggression::HandleAggresion(std::shared_ptr<Creature> creature) {
    Creatures::Human* human = dynamic_cast<Creatures::Human*>(creature.get());

    if (human->creatureAttributes.isAggressiveToPlayer) {
        if (IsRectCollidinWithTriangle(Player::GetModel(), human->sightLine)) {
            human->creatureAttributes.isAggroed = true;
            human->creatureAttributes.isAggroedTo = Player::GetAttributeReference();
            human->creatureAttributes.goingToPos = Player::GetPos();
        }
    }
}

void HumanAI::HandleAI(std::shared_ptr<Creatures::Creature> creature) {
    Human* human = dynamic_cast<Creatures::Human*>(creature.get());
    if (!human) return;
    if (human->entityAttributes.combatAttributes.isDead) return;

    aggressionComponent->HandleAggresion(creature);
    behavioralComponent->HandleBehavior(creature);
    movementComponent->HandleMovement(creature);
}

HumanAI::HumanAI(std::unique_ptr<BehavorialComponent> behavioralComponent_,
                 std::unique_ptr<ItemLogicComponent> itemLogicComponent_,
                 std::unique_ptr<MovementPatternComponent> movementComponent_,
                 std::unique_ptr<AggressionComponent> aggressionComponent_)
    : AIComponent(std::move(behavioralComponent_),
                  std::move(itemLogicComponent_),
                  std::move(movementComponent_),
                  std::move(aggressionComponent_)) {}

}  // namespace Creatures::Components
