#pragma once

#include "../../../items/Item.hh"
#include "../../EntityAttributes.hh"
#include "../Attack.hh"

namespace Attacks {
class WoodenArrow : public Arrow {
   public:
    inline static void Create(Items::ItemStats itemStats_, CreatureAttributes* entityOrigin_, float angle_) {
        std::shared_ptr<WoodenArrow> atk(new WoodenArrow());
        AttackHandler::PushToAttackVector(atk);
        atk->Init(itemStats_, entityOrigin_, angle_, entityOrigin_->positionNow, Vec2<float>{30.f, 10.f}, Vec2<float>{400, 400});
    }

    inline ~WoodenArrow() = default;

   private:
    inline WoodenArrow() = default;
};
}  // namespace Attacks
