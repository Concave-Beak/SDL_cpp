#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack.hh"
// #include "../../../include/game/entities/ItemEntity.hh"

// TODO
void Item::Drop(Matrix2D<Item>* inventory) {
    (void)inventory;
    // ItemEntity(*this);
}

void Item::Attack(Entity* entityOrigin, const Vec2<float>& spawnPos, float angle) {
    Attack::AttackType atkType = Attack::SWORD_SLASH;
    if (itemType == ItemType::SHORT_BOW) atkType = Attack::ARROW;

    new class Attack(entityOrigin, damage, spawnPos, atkType, angle, 1000);
}
