#include "../../../../include/game/entities/Attack/AttackFactory.hh"

#include "../../../../include/game/entities/Attack/Weapons/Melee.hh"
#include "../../../../include/game/entities/Attack/Weapons/Ranged.hh"

namespace Items {
AttackFactory::AttackFactory() {
    RegisterAll();
}

AttackFactory AttackFactory::instance = AttackFactory();
AttackFactory& AttackFactory::Instance() { return instance; }

void AttackFactory::CreateAttack(Attack::AttackType atkType, ItemStats itemStats, Entity* entityOrigin, float angle) {
    attackFactoryMap::iterator it = attackCreators.find(atkType);
    if (it != attackCreators.end()) {
        it->second(itemStats, entityOrigin, angle);
    }
}

void AttackFactory::RegisterAll() {
    instance.RegisterItem(Attack::AttackType::ARROW_PROJECTILE, [](ItemStats itemStats_, Entity* entityOrigin_, float angle_) {
        new Attack::WoodenArrow(itemStats_, entityOrigin_, angle_);
    });
    instance.RegisterItem(Attack::AttackType::SWORD_SLASH, [](ItemStats itemStats_, Entity* entityOrigin_, float angle_) {
        new Attack::ShortSwordSwing(itemStats_, entityOrigin_, angle_);
    });
}

void AttackFactory::RegisterItem(Attack::AttackType atkType, std::function<void(ItemStats, Entity*, float)> constructor) {
    attackCreators.emplace(atkType, constructor);
}
}  // namespace Items
