#pragma once

#include "../../Entity.hh"
#include "../Attacks.hh"

namespace Attack {
class WoodenArrow : public Attacks::ArrowProjectile {
   public:
    inline WoodenArrow(Entity *entityOrigin_, float angle_) {
        angle = angle_;

        positionNow = entityOrigin_->GetPos();  // TODO

        velocityNow = {
            400 * cos(angle_),
            400 * sin(angle_),
        };

        model = {0, 0, 30, 10};

        type = EntityType::ARROW;

        weaponStats.entityOrigin = entityOrigin_;
    }

    inline ~WoodenArrow() = default;
};
}  // namespace Attack
