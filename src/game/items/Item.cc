#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack/AttackFactory.hh"
// #include "../../../include/game/entities/ItemEntity.hh"

// TODO
void Item::Drop(Matrix2D<Item>* inventory) {
    (void)inventory;
    // ItemEntity(*this);
}

void Item::Attack(Entity* entityOrigin, float angle) {
    AttackFactory::Instance().CreateAttack(Attack::AttackType::SWORD_SLASH, entityOrigin, angle);
}
