#pragma once

#include <algorithm>
#include <memory>

#include "../Creature.hh"

namespace Creatures {
class Human;

namespace Components {
class HumanBehavior : public BehavorialComponent {
   public:
    void HandleBehavior(std::shared_ptr<Creature> creature) override;
};

class HumanItemLogic : public ItemLogicComponent {
   public:
    void HandleItemLogic(std::shared_ptr<Creature> creature) override;
};

class HumanMovementPattern : public MovementPatternComponent {
   public:
    void HandleMovement(std::shared_ptr<Creature> creature, uint32_t directionFlag) override;
};

class HumanAI : public AIComponent {
   public:
    void HandleAI(std::shared_ptr<Creatures::Creature> creature) override;

    HumanAI(std::unique_ptr<BehavorialComponent> behavioralComponent_, std::unique_ptr<ItemLogicComponent> itemLogicComponent_, std::unique_ptr<MovementPatternComponent> movementComponent_);
};

}  // namespace Components

class Human : public Creature {
    friend class Components::HumanBehavior;
    friend class Components::HumanItemLogic;
    friend class Components::HumanMovementPattern;
    friend class Components::HumanAI;

   public:
    virtual ~Human() = default;

   protected:
    Human(std::unique_ptr<Components::AIComponent> aiComponent);
};  // Used just to be handled by the components

}  // namespace Creatures
