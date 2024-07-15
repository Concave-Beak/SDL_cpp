
#include "../Creature.hh"

namespace Creatures {

class Human : public Creature {
   public:
    inline static std::shared_ptr<Creature> Create(Vec2<float> spawnPos_) {
        std::shared_ptr<Human> creature(new Human(spawnPos_));
        CreatureHandler::PushToCreatureVector(creature);
        return creature;
    }

    ~Human() = default;

   private:
    inline Human(Vec2<float> spawnPos_) {
        creatureAttributes.positionNow = spawnPos_;
        creatureAttributes.model = {static_cast<int>(spawnPos_.x), static_cast<int>(spawnPos_.y), 0, 0};
        visionConeAngle = 5;
        visionConeRange = 1000;
        GenerateModelRandomness({&creatureAttributes.model.w, &creatureAttributes.model.h}, {70, 70}, 10);
    }

   private:
};

}  // namespace Creatures
