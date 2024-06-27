#pragma once

#include <functional>
#include <unordered_map>

#include "Attacks.hh"

class AttackFactory {
    using attackFactoryMap = std::unordered_map<Attacks::AttackType, std::function<void(Entity*, float)>>;

   public:
    static AttackFactory& Instance();

    void CreateAttack(Attacks::AttackType atkType, Entity* entity, float angle);

   private:
    static AttackFactory instance;

    attackFactoryMap attackCreators = {};

   private:
    AttackFactory();
    ~AttackFactory() = default;

    void RegisterAll();
    void RegisterItem(Attacks::AttackType atkType, std::function<void(Entity*, float)> constructor);
};
