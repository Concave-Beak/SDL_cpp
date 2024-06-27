#pragma once

#include "../Item.hh"

class ShortSword : public Item {
   public:
    inline ShortSword() {
        ID = ItemID::BOW_AND_ARROW;
        name = "Bow and Arrow";
        itemType = ItemType::SHORT_BOW;
        // atkType = Attack::AttackType::ARROW;
        // attackTypeAliveTime = 1000;
        damage = 10;
        armorPenetration = 10;
        durability = 10;
        invetoryWeight = 1;
    }
    ~ShortSword() = default;
};
