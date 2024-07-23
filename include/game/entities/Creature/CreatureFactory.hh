#pragma once

#include <functional>
#include <unordered_map>

#include "./Creature.hh"

namespace Creatures {
class CreatureFactory {
    typedef std::unordered_map<CreatureType, std::function<void(Vec2<float> spawnPos)>> creatureFactoryMap;

   public:
    static CreatureFactory& Instance();

    void CreateCreature(CreatureType, Vec2<float> spawnPos);

   private:
    static CreatureFactory instance;

    creatureFactoryMap creatureCreators;

   private:
    CreatureFactory();
    ~CreatureFactory() = default;

   private:
    void RegisterAll();

    void RegisterCreature(CreatureType type, std::function<void(Vec2<float>)> constructor);
};

}  // namespace Creatures
