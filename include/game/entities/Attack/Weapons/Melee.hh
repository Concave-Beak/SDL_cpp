#pragma once

#include "../../../items/Item.hh"
#include "../Attack.hh"

namespace Attacks {

class ShortSwordSwing : public Swing {
   public:
    inline static void Create(Items::ItemStats itemStats_, Entity* entityOrigin_, float angle_) {
        std::shared_ptr<ShortSwordSwing> atk(new ShortSwordSwing());
        atk->Init(itemStats_, entityOrigin_, angle_, entityOrigin_->GetPos(), {32, 64});
        Entity::PushToEntityVector(atk);
    }

    ~ShortSwordSwing() = default;

   private:
    ShortSwordSwing() = default;
};

}  // namespace Attacks
