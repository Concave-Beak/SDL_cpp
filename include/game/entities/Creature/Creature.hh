#pragma once

#include <functional>
#include <unordered_map>

#include "../../../../lib/utils/math_utils.hh"
#include "../../items/Inventory.hh"
#include "../Entity.hh"

namespace Creatures {

enum class CreatureID {
    HUMAN = 0,
};

class Creature : protected Entity {
   public:
    Creature(Vec2<float> spawnPos_);
    virtual ~Creature() = default;

   protected:
    enum class AgressionType {
        NOT_SET,
        PASSIVE = 0,
        NEUTRAL,
        AGRESSIVE,
    };
    AgressionType agressionType;

    Vec2<float> spawnPos = {};

    Vec2<float> runningSpeed;
    Vec2<float> walkingSpeed;

    Triangle<float> visionCone;
    float visionConeAngle;  // Angle that the vision cone opens
    int visionConeRange;

    bool isWandering = true;

    Items::Inventory inventory;

   protected:
    void ResetVisionCone(Creature* npc);

    // Generates a randomness in a models width and height
    Vec2<int> GenerateRandomnessInModel(Vec2<int> modelWH, int variation);

   private:
    friend class CreatureFactory;
};

class CreatureFactory {
    typedef std::unordered_map<CreatureID, std::function<void(Vec2<float> spawnPos)>> creatureFactoryMap;

   public:
    static CreatureFactory& Instance();

    void CreateCreature(CreatureID id, Vec2<float> spawnPos);

   private:
    static CreatureFactory instance;

    creatureFactoryMap creatureCreators;

   private:
    CreatureFactory();
    ~CreatureFactory() = default;

   private:
    void RegisterAll();

    void RegisterCreature(CreatureID id, std::function<void(Vec2<float>)> constructor);
};

}  // namespace Creatures
