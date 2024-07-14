#include "../Creature.hh"

namespace Creatures {

class Human : public Creature {
   public:
    inline static std::shared_ptr<Creature> Create(Vec2<float> spawnPos_) {
        std::shared_ptr<Human> creature(new Human(spawnPos_));
        CreatureHandler::PushToCreatureEntity(creature);
        return creature;
    }

    ~Human() = default;

   private:
    inline Human(Vec2<float> spawnPos_) {
        entityAttributes.positionNow = spawnPos_;
        entityAttributes.model = {static_cast<int>(spawnPos_.x), static_cast<int>(spawnPos_.y), 0, 0};
        GenerateModelRandomness({&entityAttributes.model.w, &entityAttributes.model.h}, {70, 70}, 10);
    }

   private:
};

}  // namespace Creatures
