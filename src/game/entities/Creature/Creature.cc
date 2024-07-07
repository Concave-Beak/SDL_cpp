#include "../../../../include/game/entities/Creature/Creature.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <cstdio>
#include <cstdlib>

#include "../../../../include/game/entities/Creature/Neutral/Human.hh"

namespace Creatures {
Creature::Creature(Vec2<float> spawnPos_) : spawnPos(spawnPos_), inventory(0, 0) {}

void Creature::ResetVisionCone(Creature* npc) {
    float halfAngle = npc->visionConeAngle / 2.0f;
    float angleA = float(atan2(0.0f, npc->visionConeRange)) + halfAngle;
    float angleB = float(atan2(0.0f, npc->visionConeRange)) - halfAngle;

    Vec2<float> posNpc = npc->Entity::GetPos() + Vec2<float>{float(npc->model.w), 0};
    Vec2<float> pointA = {-(cos(angleA) * float(npc->visionConeRange)), sin(angleA) * float(npc->visionConeRange)};
    Vec2<float> pointB = {-(cos(angleB) * float(npc->visionConeRange)), sin(angleB) * float(npc->visionConeRange)};
    if (npc->Entity::GetFacingDirection() == Direction::LEFT) {
        pointA = {cos(angleA) * float(npc->visionConeRange), sin(angleA) * float(npc->visionConeRange)};
        pointB = {cos(angleB) * float(npc->visionConeRange), sin(angleB) * float(npc->visionConeRange)};
        posNpc = npc->Entity::GetPos();
    }

    npc->visionCone = {
        .a = posNpc + pointA,
        .b = posNpc + pointB,
        .c = posNpc  // Head of npc
    };
}

Vec2<int> Creature::GenerateRandomnessInModel(Vec2<int> modelWH, int variation) {
    Vec2<int> newHW = {};
    newHW.x = rand() % variation + modelWH.x;
    newHW.y = rand() % variation + modelWH.y;
    newHW.x *= rand() % 2;
    newHW.y *= rand() % 2;

    return newHW;
}

//------------------------------------------------------------------------------

CreatureFactory::CreatureFactory() {
    instance.RegisterAll();
};

CreatureFactory CreatureFactory::instance = CreatureFactory();
CreatureFactory& CreatureFactory::Instance() { return instance; }

void CreatureFactory::RegisterAll() {
    instance.RegisterCreature(CreatureID::HUMAN, [](Vec2<float> spawnPos) {
        new Human(spawnPos);
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
