#include "../../../../include/game/entities/Creature/Creature.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <memory>

#include "../../../../include/game/entities/Creature/Neutral/Human.hh"

namespace Creatures {

std::shared_ptr<Creature> Creature::Create() {
    std::shared_ptr<Creature> creature(new Creature());
    Entity::PushToEntityVector(creature);
    return creature;
}

void Creature::ResetVisionCone(Creature *npc) {
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

void Creature::GenerateModelRandomness(Vec2<int *> modelWH, Vec2<int> defaultValue, int variation) {
    *modelWH.x = rand() % variation + defaultValue.x;
    *modelWH.y = rand() % variation + defaultValue.y;
    *modelWH.x *= rand() % 2;
    *modelWH.y *= rand() % 2;
}

void Creature::Delete(std::shared_ptr<Creature> creature) {
    creatureVector.erase(std::remove(creatureVector.begin(), creatureVector.end(), creature), creatureVector.end());
    Entity::Delete(creature);
}

}  // namespace Creatures
