#pragma once

#include <memory>
#include <vector>

#include "../../../lib/utils/math_utils.hh"
#include "../items/Item.hh"
#include "./Entity.hh"

class NPC : protected Entity {
   public:
    NPC(Entity::EntityType type, Vec2<float> startPos_);
    ~NPC();

    static void Handle(SDL_Renderer* renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox);

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

    Matrix2D<std::shared_ptr<Item>> inventory = Matrix2D<std::shared_ptr<Item>>{
        {
            // ItemFactory::Instance().CreateItem(Item::ItemID::BOW_AND_ARROW),
        },
    };

   private:
    static std::vector<NPC*> npcVector;

    static void HandleMovement(NPC* npc, SDL_Renderer* renderer, Vec2<float> playerPos, Vec2<float> cameraPos, Vec2<int> playerHitbox);
    static void FollowPlayer(Vec2<float> playerPos, NPC* npc);
    static void ResetVisionCone(NPC* npc);

    void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) override;
};
