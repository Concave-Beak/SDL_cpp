#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack/AttackFactory.hh"
// #include "../../../include/game/entities/ItemEntity.hh"

namespace Items {

Item::Item(ItemStats stats) : itemStats(stats) {}

ItemStats::ItemStats() : weight(0), damage(0), armorPenetration(0), isUsable(false), useCooldown(0), attackCooldownInTicks(0), chargeRate(0), chargeNow(0), durability(0) {}

ItemStats::ItemStats(float weight_, float damage_, float armorPenetration_, Uint32 cooldownInTicks_, int durability_, float chargeRate_) : weight(weight_), damage(damage_), armorPenetration(armorPenetration_), attackCooldownInTicks(cooldownInTicks_), durability(durability_), chargeRate(chargeRate_) {}

//------------------------------------------------------------------------------

ItemStats Item::GetItemStats() { return itemStats; }

// to be done
void Item::Drop(Matrix2D<Item>* inventory) {
    (void)inventory;
    // ItemEntity(*this);
}

void Item::ChargeAttack() {
    itemStats.chargeNow += itemStats.chargeRate;
    if (itemStats.chargeNow > 100) {
        itemStats.chargeNow = 100;
    }
}

void Item::ReleaseAttack(Entity* attackOrigin, float angle, Uint32* entityCooldown) {
    if (*entityCooldown > SDL_GetTicks()) return;

    Attacks::AttackFactory::Instance().CreateAttack(Attacks::AttackType::ARROW_PROJECTILE, itemStats, attackOrigin, angle);
    *entityCooldown = SDL_GetTicks() + itemStats.attackCooldownInTicks;
    itemStats.chargeNow = 0;
}

}  // namespace Items
