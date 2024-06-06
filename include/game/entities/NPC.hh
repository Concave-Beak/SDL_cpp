#pragma once

#include <vector>

#include "../../../lib/utils/math_utils.hh"
#include "../items/Item.hh"
#include "./Entity.hh"

class NPC {
   public:
    NPC(Entity::EntityID id, Vec2<float> startPos_);
    ~NPC();

    Vec2<float> GetPos();
    Vec2<float> GetVelocityNow();

    Vec2<int> GetHitbox();

    Entity entity;

    static void HandleMovement(SDL_Renderer* renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox);
    static void FollowPlayer(Vec2<float> playerPos, NPC* npc);
    static void ResetVisionCone(NPC* npc);

   private:
    Vec2<float> spawnPos = {};

    Vec2<float> runningSpeed;
    Vec2<float> walkingSpeed;

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

    Uint32 healthMax;
    Uint32 healthNow;

    Matrix2D<Item> inventory = Matrix2D<Item>{
        {
            Item(Item::SHORTSWORD),
            Item(Item::BOW_AND_ARROW),
        },
    };

    static std::vector<NPC*> npcVector;
};
