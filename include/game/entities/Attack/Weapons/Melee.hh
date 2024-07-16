#pragma once

#include "../../../items/Item.hh"
#include "../Attack.hh"

namespace Attacks {

class ShortSwordSwing : public MeleeSwing {
   public:
    inline static void Create(Items::ItemStats itemStats_, CreatureAttributes* entityOrigin_, float angle_) {
        std::shared_ptr<ShortSwordSwing> atk(new ShortSwordSwing());
        atk->Init(itemStats_, entityOrigin_, angle_, entityOrigin_->positionNow, {32, 64});
        AttackHandler::PushToAttackVector(atk);
    }

    ~ShortSwordSwing() = default;

   private:
    ShortSwordSwing() = default;
};

}  // namespace Attacks
