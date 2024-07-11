#pragma once

#include <functional>
#include <unordered_map>

#include "./Creature.hh"

namespace Creatures {
class CreatureFactory {
    typedef std::unordered_map<CreatureID, std::function<void(Vec2<float> spawnPos)>> creatureFactoryMap;

   public:
    static CreatureFactory& Instance();

    void CreateCreature(CreatureID id, Vec2<float> spawnPos);

   private:
    static CreatureFactory instance;

    creatureFactoryMap creatureCreators;

   private:
    CreatureFactory();
    ~CreatureFactory() = default;

   private:
    void RegisterAll();

    void RegisterCreature(CreatureID id, std::function<void(Vec2<float>)> constructor);
};

}  // namespace Creatures
