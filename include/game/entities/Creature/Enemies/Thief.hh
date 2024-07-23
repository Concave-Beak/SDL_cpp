#pragma once

#include "../Creature.hh"
#include "../Neutral/Human.hh"

namespace Creatures {

class Thief : public Human {
   public:
    static std::shared_ptr<Thief> Create(Vec2<float> spawnPos_);

    ~Thief() = default;

   private:
    Thief(Vec2<float> spawnPos_);
};

}  // namespace Creatures
