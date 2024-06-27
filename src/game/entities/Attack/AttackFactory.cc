#include "../../../../include/game/entities/Attack/AttackFactory.hh"

#include "../../../../include/game/entities/Attack/Weapons/Ranged.hh"

AttackFactory::AttackFactory() {
    RegisterAll();
}

AttackFactory AttackFactory::instance = AttackFactory();
AttackFactory& AttackFactory::Instance() { return instance; }

void AttackFactory::CreateAttack(Attacks::AttackType atkType, Entity* entityOrigin, float angle) {
    attackFactoryMap::iterator it = attackCreators.find(atkType);
    if (it != attackCreators.end()) {
        it->second(entityOrigin, angle);
    }
}

void AttackFactory::RegisterAll() {
    instance.RegisterItem(Attacks::AttackType::ARROW_PROJECTILE, [](Entity* entityOrigin_, float angle_) {
        new Attack::WoodenArrow(entityOrigin_, angle_);
    });
    // instance.RegisterItem(AttackType::SWORD_SLASH, [](Entity* entityOrigin_, float angle_) {
    //     new Attacks::Arrow(entityOrigin_, angle_);
    // });
}

void AttackFactory::RegisterItem(Attacks::AttackType atkType, std::function<void(Entity*, float)> constructor) {
    attackCreators.emplace(atkType, constructor);
}
