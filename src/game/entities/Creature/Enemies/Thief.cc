#include "../../../../../include/game/entities/Creature/Enemies/Thief.hh"

#include <memory>

#include "../../../../../include/game/entities/Creature/Neutral/Human.hh"

namespace Creatures {

std::shared_ptr<Thief> Thief::Create(Vec2<float> spawnPos_) {
    std::shared_ptr<Thief> creature(new Thief(spawnPos_));
    CreatureHandler::PushToCreatureVector(creature);
    return creature;
}

Thief::Thief(Vec2<float> spawnPos_)
    : Human(std::make_unique<Components::HumanAI>(
          std::make_unique<Components::HumanBehavior>(),
          std::make_unique<Components::HumanItemLogic>(),
          std::make_unique<Components::HumanMovement>(),
          std::make_unique<Components::HumanAggression>())) {
    entityAttributes.positionNow = spawnPos_;
    entityAttributes.model = {static_cast<int>(spawnPos_.x), static_cast<int>(spawnPos_.y), 0, 0};
    // creatureAttributes.aggressionTypeAgainstPlayer = CreatureAttributes::AggressionType::AGGRESSIVE;
    creatureAttributes.isAggressiveToPlayer = true;
    visionConeAngle = 5;
    visionConeRange = 1000;
    entityAttributes.walkingSpeed = {5, 100};
    entityAttributes.runningSpeed = {10, 100};
    GenerateModelRandomness({&entityAttributes.model.w, &entityAttributes.model.h}, {70, 70}, 10);
}

}  // namespace Creatures
