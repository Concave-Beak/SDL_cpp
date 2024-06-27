#include "../../../include/game/entities/NPC.hh"

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../../../lib/utils/sdl_utils.hh"

std::vector<NPC *> NPC::npcVector = {};

NPC::NPC(Entity::EntityType type_, Vec2<float> spawnPos_) : spawnPos(spawnPos_) {
    model = {0, 0, 64, 64};
    isWandering = true;  // Placeholder
    runningSpeed = {10, 200};
    walkingSpeed = {5, 150};
    visionConeAngle = 20;
    visionConeRange = 1000;
    Entity::Init(type_);
    npcVector.push_back(this);
}

NPC::~NPC() {}

void NPC::ResetVisionCone(NPC *npc) {
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

void NPC::FollowPlayer(Vec2<float> playerPos, NPC *npc) {
    Direction dir = Direction::RIGHT;
    if (playerPos.x < npc->Entity::GetPos().x) {
        dir = Direction::LEFT;
    }
    npc->Entity::Move(dir, npc->runningSpeed, false);
}

void NPC::Handle(SDL_Renderer *renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox) {
    for (NPC *npc : npcVector) {
        HandleMovement(npc, renderer, playerPos, cameraPos, playerHitbox);
    }
}

void NPC::HandleMovement(NPC *npc, SDL_Renderer *renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox) {
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

void NPC::Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) {
    SDL_Rect npcModel = {
        // rectangles for now, because I dont have proper models
        (int)positionNow.x - cameraPos.x,
        (int)positionNow.y - cameraPos.y,
        model.w,
        model.h,
    };
    if (isDead) {
        scc(SDL_SetRenderDrawColor(renderer, WHITE, 0xff)).Handle();
    } else {
        scc(SDL_SetRenderDrawColor(renderer, YELLOW, 0xff)).Handle();
    }
    scc(SDL_RenderFillRect(renderer, &npcModel)).Handle();
}
