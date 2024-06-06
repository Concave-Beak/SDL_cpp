#include "../../../include/game/entities/NPC.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../../../lib/utils/sdl_utils.hh"

std::vector<NPC *> NPC::npcVector = {};

NPC::NPC(Entity::EntityID id, Vec2<float> spawnPos_) : entity(Entity::GENERIC_HUMANOID_ENEMY, spawnPos_, {70, 70}), spawnPos(spawnPos_) {
    (void)id;
    this->isWandering = true;  // Placeholder
    this->runningSpeed = {0.8f, 200};
    this->walkingSpeed = {0.2f, 150};
    this->visionConeAngle = 20;
    this->visionConeRange = 1000;
    this->entity.Move(Direction::LEFT, {0, 0}, false);
    npcVector.push_back(this);
}

NPC::~NPC() {}

Vec2<float> NPC::GetPos() { return entity.GetPos(); }

Vec2<float> NPC::GetVelocityNow() { return entity.GetVelocityNow(); }

Vec2<int> NPC::GetHitbox() { return entity.GetHitbox(); }

void NPC::ResetVisionCone(NPC *npc) {
    float halfAngle = npc->visionConeAngle / 2.0f;
    float angleA = float(atan2(0.0f, npc->visionConeRange)) + halfAngle;
    float angleB = float(atan2(0.0f, npc->visionConeRange)) - halfAngle;

    Vec2<float> posNpc = npc->entity.GetPos() + Vec2<float>{float(npc->GetHitbox().x), 0};
    Vec2<float> pointA = {-(cos(angleA) * float(npc->visionConeRange)), sin(angleA) * float(npc->visionConeRange)};
    Vec2<float> pointB = {-(cos(angleB) * float(npc->visionConeRange)), sin(angleB) * float(npc->visionConeRange)};
    if (npc->entity.GetFacingDirection() == Direction::LEFT) {
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

void NPC::FollowPlayer(Vec2<float> playerPos, NPC *npc) {
    Direction dir = RIGHT;
    if (playerPos.x < npc->entity.GetPos().x) {
        dir = LEFT;
    }
    npc->entity.Move(dir, npc->runningSpeed, false);
}

void NPC::HandleMovement(SDL_Renderer *renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox) {
    for (NPC *npc : npcVector) {
        ResetVisionCone(npc);
        npc->visionCone.Draw(renderer, cameraPos, SDL_Color{BLACK, 0x80});
        if (npc->isWandering) {
            if (IsPointInTriangle(playerPos, npc->visionCone) || IsPointInTriangle(playerPos + Vec2<float>(playerHitbox), npc->visionCone)) {
                npc->isWandering = false, npc->isAggroed = true;
            }
        }
        if (npc->isAggroed) {
            FollowPlayer(playerPos, npc);
        }
    }
}
