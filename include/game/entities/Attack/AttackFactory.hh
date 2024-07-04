#pragma once

#include <functional>
#include <unordered_map>

#include "../../items/Item.hh"
#include "Attack.hh"

namespace Items {
class AttackFactory {
    using attackFactoryMap = std::unordered_map<Attack::AttackType, std::function<void(ItemStats, Entity*, float)>>;

   public:
    static AttackFactory& Instance();

    void CreateAttack(Attack::AttackType atkType, ItemStats itemStats, Entity* entity, float angle);

   private:
    static AttackFactory instance;

    attackFactoryMap attackCreators = {};

   private:
    AttackFactory();
    ~AttackFactory() = default;

    void RegisterAll();
    void RegisterItem(Attack::AttackType atkType, std::function<void(ItemStats, Entity*, float)> constructor);
};
}  // namespace Items
