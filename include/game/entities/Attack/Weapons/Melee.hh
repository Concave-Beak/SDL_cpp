#pragma once

#include "../../../items/Item.hh"
#include "../Attack.hh"

namespace Attacks {

class ShortSwordSwing : public Swing {
   public:
    inline ShortSwordSwing(Items::ItemStats itemStats_, std::shared_ptr<Entity> entityOrigin_, float angle_) : Swing(itemStats_, entityOrigin_, angle_, entityOrigin_->GetPos(), {32, 64}) {
    }
};

}  // namespace Attacks
