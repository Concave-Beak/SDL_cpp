#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
#include <memory>

#include "../../../../../lib/utils/math_utils.hh"
#include "../../../../main/Level.hh"

namespace Creatures {
class Creature;
}

// Base definitions of creature components.
// These Components basically dictate how to creature is going to interact with it's
// world and or the player.

namespace Creatures::Components {

// BehavorialComponent | Dictates basic behavior to player and other creatures
class BehavorialComponent {
   public:
    virtual ~BehavorialComponent() = default;

    virtual void HandleBehavior(std::shared_ptr<Creature> creature) = 0;

    BehavorialComponent() = default;
};

// ItemLogicComponent | Dictates how will interact to Items, eg: equip/discard/ignore
class ItemLogicComponent {
   public:
    virtual ~ItemLogicComponent() = default;

    virtual void HandleItemLogic(std::shared_ptr<Creature> creature) = 0;
};

// MovementPatternComponent | Dictates how will move
class MovementPatternComponent {
   public:
    virtual ~MovementPatternComponent() = default;

    virtual void HandleMovement(std::shared_ptr<Creature> creature, uint32_t directionFlag) = 0;
};

// AIComponent | Calls all of the components accordingly
class AIComponent {
   public:
    virtual void HandleAI(std::shared_ptr<Creatures::Creature> creature) = 0;
    virtual ~AIComponent() = default;

   protected:
    AIComponent(std::unique_ptr<BehavorialComponent> behavioralComponent_, std::unique_ptr<ItemLogicComponent> itemLogicComponent_, std::unique_ptr<MovementPatternComponent> movementComponent_);

    std::unique_ptr<BehavorialComponent> behavioralComponent;
    std::unique_ptr<ItemLogicComponent> itemLogicComponent;
    std::unique_ptr<MovementPatternComponent> movementComponent;
};

// To be improved: the classes below it will remain as a defaulted component.
class DrawingComponent {
   public:
    virtual void Draw(SDL_Renderer* renderer, Vec2<float> cameraPos, std::shared_ptr<Creature> creature);
    // For now it will only be the basic drawing method in all classes.
    // Later on other creatures might have particles and whatever and this component is for that
};

class PhysicsComponent {
   public:
    virtual void HandlePhysics(std::shared_ptr<Creatures::Creature> creature, float timeDelta, float timeMultiplier);
};

class CollisionComponent {
   public:
    virtual void HandleCollision(std::shared_ptr<Creatures::Creature> creature, float timeDelta, float timeMultiplier);

   private:
    virtual void HandleVerticalCollision(std::shared_ptr<Creatures::Creature> creature, SDL_Rect entityRect, const LevelItem& levelItem, float timeDelta, float timeMultiplier);

    virtual void HandleHorizontalCollision(std::shared_ptr<Creatures::Creature> creature, SDL_Rect entityRect, const LevelItem& levelItem, float timeDelta, float timeMultiplier);
};

}  // namespace Creatures::Components
