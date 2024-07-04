#pragma once

#include "../../../items/Item.hh"
#include "../Attack.hh"

namespace Attack {

class ShortSwordSwing : public Attack::Swing {
   public:
    inline ShortSwordSwing(Items::ItemStats itemStats_, Entity* entityOrigin_, float angle_) : Attack::Swing(itemStats_, entityOrigin_, angle_, entityOrigin_->GetPos(), {32, 64}) {
    }
};

}  // namespace Attack
