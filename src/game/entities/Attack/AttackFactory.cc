#include "../../../../include/game/entities/Attack/AttackFactory.hh"

#include "../../../../include/game/entities/Attack/Weapons/Melee.hh"
#include "../../../../include/game/entities/Attack/Weapons/Ranged.hh"

namespace Attacks {
AttackFactory::AttackFactory() {
    RegisterAll();
}

AttackFactory AttackFactory::instance = AttackFactory();
AttackFactory& AttackFactory::Instance() { return instance; }

void AttackFactory::CreateAttack(AttackType atkType, Items::ItemStats itemStats, Entity* entityOrigin, float angle) {
    attackFactoryMap::iterator it = attackCreators.find(atkType);
    if (it != attackCreators.end()) {
        it->second(itemStats, entityOrigin, angle);
    }
}

void AttackFactory::RegisterAll() {
    instance.RegisterItem(AttackType::ARROW_PROJECTILE, [](Items::ItemStats itemStats_, Entity* entityOrigin_, float angle_) {
        new WoodenArrow(itemStats_, entityOrigin_, angle_);
    });
    instance.RegisterItem(AttackType::SWORD_SLASH, [](Items::ItemStats itemStats_, Entity* entityOrigin_, float angle_) {
        new ShortSwordSwing(itemStats_, entityOrigin_, angle_);
    });
}

void AttackFactory::RegisterItem(AttackType atkType, std::function<void(Items::ItemStats, Entity*, float)> constructor) {
    attackCreators.emplace(atkType, constructor);
}
}  // namespace Items
