#include "../../../../include/game/entities/Creature/CreatureFactory.hh"

#include "../../../../include/game/entities/Creature/Creature.hh"
#include "../../../../include/game/entities/Creature/Enemies/Thief.hh"

namespace Creatures {

CreatureFactory::CreatureFactory() {
    instance.RegisterAll();
};

CreatureFactory CreatureFactory::instance = CreatureFactory();
CreatureFactory& CreatureFactory::Instance() { return instance; }

void CreatureFactory::RegisterAll() {
    instance.RegisterCreature(CreatureType::THIEF, [](Vec2<float> spawnPos) {
        Thief::Create(spawnPos);
    });
}
void CreatureFactory::RegisterCreature(CreatureType id, std::function<void(Vec2<float>)> constructor) {
    instance.creatureCreators.emplace(id, constructor);
}

void CreatureFactory::CreateCreature(CreatureType id, Vec2<float> spawnPos) {
    creatureFactoryMap::iterator it = creatureCreators.find(id);
    if (it != creatureCreators.end()) {
        it->second(spawnPos);
    }
}

}  // namespace Creatures
