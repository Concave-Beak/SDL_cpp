#include "../Creature.hh"

namespace Creatures {

class Human : public Creature {
   public:
    inline Human(Vec2<float> spawnPos_) : Creature(spawnPos_) {
        positionNow = spawnPos_;
        model = {static_cast<int>(spawnPos_.x), static_cast<int>(spawnPos_.y), 70, 70};
        Vec2<int> modelWH = GenerateRandomnessInModel({model.w, model.h}, 10);
        model.w = modelWH.x;
        model.h = modelWH.y;
    }
    ~Human() = default;

   private:
};

}  // namespace Creatures
