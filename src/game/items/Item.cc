#include "../../../include/game/items/Item.hh"

#include <SDL2/SDL_timer.h>

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

void Item::ReleaseAttack(CreatureAttributes* entityAttribute, CreatureAttributes::CombatAttributes* combatAttribute, float angle) {
    if (combatAttribute->itemUseCooldown > SDL_GetTicks()) return;

    Attacks::AttackFactory::Instance().CreateAttack(AttackType::SWORD_SLASH, itemStats, entityAttribute, angle);
    combatAttribute->itemUseCooldown = SDL_GetTicks() + itemStats.attackCooldownInTicks;
    itemStats.chargeNow = 0;
}

}  // namespace Items
