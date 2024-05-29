#pragma once

#include <vector>

#include "./Entity.hh"

class NPC {
   public:
    NPC(Entity::EntityID id);
    ~NPC();

    Vector2<float> GetPos();
    Vector2<float> GetVelocity();

    Vector2<int> GetHitbox();

    Entity entity;

    static void HandleMovement(Vector2<float> playerPos);

   private:
    static std::vector<NPC*> npcVector;
};
