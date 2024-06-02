#pragma once

#include <vector>

#include "../../lib/utils/math_utils.hh"
#include "./Entity.hh"

class NPC {
   public:
    NPC(Entity::EntityID id, Vector2<float> startPos_);
    ~NPC();

    Vector2<float> GetPos();
    Vector2<float> GetVelocity();

    Vector2<int> GetHitbox();

    Entity entity;

    static void HandleMovement(SDL_Renderer* renderer, Vector2<float> playerPos, Vector2<float> cameraPos, Vector2<int> playerHitbox);
    static void FollowPlayer(Vector2<float> playerPos, NPC* npc);
    static void ResetVisionCone(NPC* npc);

   private:
    Vector2<float> spawnPos = {};

    Vector2<float> runningSpeed;
    Vector2<float> walkingSpeed;

    Triangle<float> visionCone;
    float visionConeAngle;  // Angle that the vision cone opens
    int visionConeRange;

    bool isWandering = true;
    bool isAggroed = false;

    enum AggressionType {
        PEACEFULL = 0,
        NEUTRAL,
        HOSTILE,
        SCARED,
    };
    AggressionType aggressionType;

    static std::vector<NPC*> npcVector;
};
