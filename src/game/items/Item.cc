#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack.hh"
#include "../../../include/game/entities/ItemEntity.hh"

Item::Item(ItemID itemID_) : ID(itemID_) {
    switch (itemID_) {
        case BOW_AND_ARROW: {
            itemTtype = SHORT_BOW;
            name = "Bow and arrow";
            damage = 10;
            armorPenetration = 1.0f;
            durability = 20;
            range = 50;

            break;
        }
        case SHORTSWORD: {
            itemTtype = ONE_HANDED_SWORD;
            name = "Short sword";
            damage = 10;
            armorPenetration = 1.0f;
            durability = 20;
            range = 50;
            break;
        }
    }
}
Item::~Item() {}

// TODO
void Item::Drop(Matrix2D<Item>* inventory) {
    ItemEntity(*this);
}

void Item::Attack(Entity* entityOrigin, const Vec2<float>& spawnPos, float angle) {
    Attack::AttackType atkType = Attack::SWORD_SLASH;
    if (this->itemTtype == ItemType::SHORT_BOW) atkType = Attack::ARROW;

    new class Attack(entityOrigin, this->damage, spawnPos, atkType, angle, 1000);
}
