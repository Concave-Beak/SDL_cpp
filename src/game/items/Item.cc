#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack/AttackFactory.hh"
// #include "../../../include/game/entities/ItemEntity.hh"

// TODO
void Item::Drop(Matrix2D<Item>* inventory) {
    (void)inventory;
    // ItemEntity(*this);
}

void Item::Attack(Entity* entityOrigin, const Vec2<float>& spawnPos, float angle) {
    // Attack::AttackType atkType = Attack::SWORD_SLASH;
    // if (itemType == ItemType::SHORT_BOW) atkType = Attack::ARROW;

    AttackFactory::Instance().CreateAttack(Attacks::AttackType::ARROW_PROJECTILE, entityOrigin, angle);
}
