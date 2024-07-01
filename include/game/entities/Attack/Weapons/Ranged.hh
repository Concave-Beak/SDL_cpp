#pragma once

#include "../../Entity.hh"
#include "../Attack.hh"

namespace Attack {
class WoodenArrow : public Attack::Arrow {
   public:
    inline WoodenArrow(Entity *entityOrigin_, float angle_) : Attack::Arrow(entityOrigin_, angle_, entityOrigin_->GetPos(), Vec2<float>{30.f, 10.f}, Vec2<float>{400, 400}) {}

    inline ~WoodenArrow() = default;
};
}  // namespace Attack
