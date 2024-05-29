#include "../../include/entities/NPC.hh"

#include <vector>

std::vector<NPC*> NPC::npcVector = {};

NPC::NPC(Entity::EntityID id) : entity(Entity::GENERIC_HUMANOID_ENEMY, {0.5, 200}, {100, 0}, {70, 70}) {
    (void)id;
    npcVector.push_back(this);
}

NPC::~NPC() {}

Vector2<float> NPC::GetPos() { return entity.GetPos(); }
Vector2<float> NPC::GetVelocity() { return entity.GetVelocity(); }

Vector2<int> NPC::GetHitbox() { return entity.GetHitbox(); }

void NPC::HandleMovement(Vector2<float> playerPos) {
    for (NPC* npc : npcVector) {
        if (playerPos.x < npc->GetPos().x) {
            npc->entity.Move(LEFT, false);
        } else {
            npc->entity.Move(RIGHT, false);
        }
        if (playerPos.y < npc->GetPos().y) {
            npc->entity.Move(UP, false);
        }
    }
}
