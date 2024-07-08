#include "../../../../include/game/entities/Attack/AttackFactory.hh"

#include "../../../../include/game/entities/Attack/Weapons/Melee.hh"
#include "../../../../include/game/entities/Attack/Weapons/Ranged.hh"

namespace Attacks {
AttackFactory::AttackFactory() {
    RegisterAll();
}

AttackFactory AttackFactory::instance = AttackFactory();
AttackFactory& AttackFactory::Instance() { return instance; }

void AttackFactory::CreateAttack(AttackType atkType, Items::ItemStats itemStats, std::shared_ptr<Entity> entity, float angle) {
    attackFactoryMap::iterator it = attackCreators.find(atkType);
    if (it != attackCreators.end()) {
        it->second(itemStats, entity, angle);
    }
}

void AttackFactory::RegisterAll() {
    instance.RegisterAttack(AttackType::ARROW_PROJECTILE, [](Items::ItemStats itemStats_, std::shared_ptr<Entity> entity_, float angle_) {
        new WoodenArrow(itemStats_, entity_, angle_);
    });
    instance.RegisterAttack(AttackType::SWORD_SLASH, [](Items::ItemStats itemStats_, std::shared_ptr<Entity> entity_, float angle_) {
        new ShortSwordSwing(itemStats_, entity_, angle_);
    });
}

void AttackFactory::RegisterAttack(AttackType atkType, std::function<void(Items::ItemStats, std::shared_ptr<Entity>, float)> constructor) {
    attackCreators.emplace(atkType, constructor);
}
}  // namespace Attacks
