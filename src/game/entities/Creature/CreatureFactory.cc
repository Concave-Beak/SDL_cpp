#include "../../../../include/game/entities/Creature/CreatureFactory.hh"

#include "../../../../include/game/entities/Creature/Creature.hh"
#include "../../../../include/game/entities/Creature/Neutral/Human.hh"

namespace Creatures {

CreatureFactory::CreatureFactory() {
    instance.RegisterAll();
};

CreatureFactory CreatureFactory::instance = CreatureFactory();
CreatureFactory& CreatureFactory::Instance() { return instance; }

void CreatureFactory::RegisterAll() {
    instance.RegisterCreature(CreatureID::HUMAN, [](Vec2<float> spawnPos) {
        Human::Create(spawnPos);
    });
}
void CreatureFactory::RegisterCreature(CreatureID id, std::function<void(Vec2<float>)> constructor) {
    instance.creatureCreators.emplace(id, constructor);
}

void CreatureFactory::CreateCreature(CreatureID id, Vec2<float> spawnPos) {
    creatureFactoryMap::iterator it = creatureCreators.find(id);
    if (it != creatureCreators.end()) {
        it->second(spawnPos);
    }
}

}  // namespace Creatures
