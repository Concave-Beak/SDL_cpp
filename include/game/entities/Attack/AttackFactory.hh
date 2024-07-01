#pragma once

#include <functional>
#include <unordered_map>

#include "Attack.hh"

class AttackFactory {
    using attackFactoryMap = std::unordered_map<Attack::AttackType, std::function<void(Entity*, float)>>;

   public:
    static AttackFactory& Instance();

    void CreateAttack(Attack::AttackType atkType, Entity* entity, float angle);

   private:
    static AttackFactory instance;

    attackFactoryMap attackCreators = {};

   private:
    AttackFactory();
    ~AttackFactory() = default;

    void RegisterAll();
    void RegisterItem(Attack::AttackType atkType, std::function<void(Entity*, float)> constructor);
};
