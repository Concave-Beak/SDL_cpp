#include "../../include/entities/NPC.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../../lib/utils/sdl_utils.hh"

std::vector<NPC *> NPC::npcVector = {};

NPC::NPC(Entity::EntityID id, Vector2<float> spawnPos_) : entity(Entity::GENERIC_HUMANOID_ENEMY, spawnPos_, {70, 70}), spawnPos(spawnPos_) {
    (void)id;
    this->isWandering = true;  // Placeholder
    this->runningSpeed = {0.8f, 200};
    this->walkingSpeed = {0.2f, 150};
    this->visionConeAngle = 20;
    this->visionConeRange = 1000;
    this->entity.Move(LEFT, {0, 0}, false);
    npcVector.push_back(this);
}

NPC::~NPC() {}

Vector2<float> NPC::GetPos() { return entity.GetPos(); }

Vector2<float> NPC::GetVelocity() { return entity.GetVelocity(); }

Vector2<int> NPC::GetHitbox() { return entity.GetHitbox(); }

void NPC::ResetVisionCone(NPC *npc) {
    float halfAngle = npc->visionConeAngle / 2.0f;
    float angleA = atan2(0.0f, npc->visionConeRange) + halfAngle;
    float angleB = atan2(0.0f, npc->visionConeRange) - halfAngle;

    Vector2<float> posNpc = npc->entity.GetPos() + Vector2<float>{float(npc->GetHitbox().x), 0};
    Vector2<float> pointA = {-(cos(angleA) * float(npc->visionConeRange)), sin(angleA) * float(npc->visionConeRange)};
    Vector2<float> pointB = {-(cos(angleB) * float(npc->visionConeRange)), sin(angleB) * float(npc->visionConeRange)};
    if (npc->entity.GetFacingDirection() == LEFT) {
        pointA = {cos(angleA) * float(npc->visionConeRange), sin(angleA) * float(npc->visionConeRange)};
        pointB = {cos(angleB) * float(npc->visionConeRange), sin(angleB) * float(npc->visionConeRange)};
        posNpc = npc->entity.GetPos();
    }

    npc->visionCone = {
        .a = posNpc + pointA,
        .b = posNpc + pointB,
        .c = posNpc  // Head of npc
    };
}

void NPC::FollowPlayer(Vector2<float> playerPos, NPC *npc) {
    Direction dir = RIGHT;
    if (playerPos.x < npc->entity.GetPos().x) {
        dir = LEFT;
    }
    npc->entity.Move(dir, npc->runningSpeed, false);
}

void NPC::HandleMovement(SDL_Renderer *renderer, Vector2<float> playerPos, Vector2<float> cameraPos, Vector2<int> playerHitbox) {
    for (NPC *npc : npcVector) {
        ResetVisionCone(npc);
        npc->visionCone.Draw(renderer, cameraPos, SDL_Color{BLACK, 0x80});
        if (npc->isWandering) {
            if (IsPointInTriangle(playerPos, npc->visionCone)) {
                npc->isWandering = false, npc->isAggroed = true;
            }
        }
        if (npc->isAggroed) {
            FollowPlayer(playerPos, npc);
        }
    }
}
