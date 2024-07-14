#pragma once

#include <functional>
#include <unordered_map>

#include "../../items/Item.hh"
#include "Attack.hh"

namespace Attacks {
class AttackFactory {
    typedef std::unordered_map<AttackType, std::function<void(Items::ItemStats, EntityAttributes*, float)>> attackFactoryMap;

   public:
    static AttackFactory& Instance();

    void CreateAttack(AttackType atkType, Items::ItemStats itemStats, EntityAttributes* entity, float angle);

   private:
    static AttackFactory instance;

    attackFactoryMap attackCreators = {};

   private:
    AttackFactory();
    ~AttackFactory() = default;

    void RegisterAll();
    void RegisterAttack(AttackType atkType, std::function<void(Items::ItemStats, EntityAttributes*, float)> constructor);
};
}  // namespace Attacks
