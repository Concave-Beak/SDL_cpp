#pragma once

#include "../Attack.hh"

namespace Attack {

class ShortSwordSwing : public Attack::Swing {
   public:
    inline ShortSwordSwing(Entity* entityOrigin_, float angle_) : Attack::Swing(entityOrigin_, angle_, entityOrigin_->GetPos(), {32, 64}) {
    }
};

}  // namespace Attack
