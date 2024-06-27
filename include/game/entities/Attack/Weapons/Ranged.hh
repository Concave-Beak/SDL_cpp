#pragma once

#include "../../Entity.hh"
#include "../Attacks.hh"

namespace Attack {
class WoodenArrow : public Attacks::ArrowProjectile {
   public:
    inline WoodenArrow(Entity *entityOrigin_, float angle_) : Attacks::ArrowProjectile(angle_, entityOrigin_->GetPos(), Vec2<float>{30.f, 10.f}, Vec2<float>{400, 400}) {
        model = {0, 0, 30, 10};  // not really used

        positionNow = entityOrigin_->GetPos();  // TODO

        type = EntityType::ARROW;

        weaponStats.entityOrigin = entityOrigin_;
    }

    inline ~WoodenArrow() = default;
};
}  // namespace Attack
