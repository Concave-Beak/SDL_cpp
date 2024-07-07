#pragma once

#include "../../../items/Item.hh"
#include "../../Entity.hh"
#include "../Attack.hh"

namespace Attacks {
class WoodenArrow : public Arrow {
   public:
    inline WoodenArrow(Items::ItemStats itemStats_, Entity *entityOrigin_, float angle_) : Arrow(itemStats_, entityOrigin_, angle_, entityOrigin_->GetPos(), Vec2<float>{30.f, 10.f}, Vec2<float>{400, 400}) {}

    inline ~WoodenArrow() = default;
};
}  // namespace Attack
