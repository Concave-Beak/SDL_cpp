#include "../../../include/game/items/Item.hh"

#include "../../../include/game/entities/Attack/AttackFactory.hh"
// #include "../../../include/game/entities/ItemEntity.hh"

namespace Items {

Item::Item(ItemStats stats) : itemStats(stats) {}

ItemStats::ItemStats(float weight_, float damage_, float armorPenetration_, Uint32 cooldownInTicks_, int durability_) : weight(weight_), damage(damage_), armorPenetration(armorPenetration_), attackCooldownInTicks(cooldownInTicks_), durability(durability_) {}

ItemStats Item::GetItemStats() { return itemStats; }

// to be done
void Item::Drop(Matrix2D<Item>* inventory) {
    (void)inventory;
    // ItemEntity(*this);
}

void Item::ChargeAttack() {
    if (itemStats.chargeNow > 100) {
        itemStats.chargeNow = 100;
        isBeingCharged = true;
        return;
    }
    itemStats.chargeNow += itemStats.chargeRate;
    isBeingCharged = true;
}

void Item::Attack(Entity* attackOrigin, float angle, Uint32* entityCooldown) {
    if (*entityCooldown > SDL_GetTicks()) return;

    if (itemStats.chargeNow == 0 || isBeingCharged) {
        ChargeAttack();
        return;
    }
    if (itemStats.chargeNow != 0 && !isBeingCharged) {
        AttackFactory::Instance().CreateAttack(Attack::AttackType::SWORD_SLASH, itemStats, attackOrigin, angle);
        *entityCooldown = SDL_GetTicks() + itemStats.attackCooldownInTicks;
    }
}

void Item::ReleaseAttack() {}
void Item::UnchargeAttack() {}
}  // namespace Items
