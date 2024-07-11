#pragma once

#include <memory>

#include "../../../../lib/utils/math_utils.hh"
#include "../../items/Inventory.hh"
#include "../Entity.hh"

namespace Creatures {

enum class CreatureID {
    HUMAN = 0,
};

class Creature : public Entity {
   public:
    static std::shared_ptr<Creature> Create();

    virtual ~Creature() = default;

   protected:
    Creature() = default;

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

    Items::Inventory inventory = Items::Inventory(0, 0);

   protected:
    void ResetVisionCone(Creature *npc);

    // Generates a randomness in a models width and height
    void GenerateModelRandomness(Vec2<int *> modelWH, Vec2<int> defaultValue, int variation);

   private:
    static inline std::vector<std::shared_ptr<Creature>> creatureVector = {};

   private:
    static void Delete(std::shared_ptr<Creature> creature);

   private:
    // void Move(const Direction &direction, const Vec2<float> &accelSpeed, const bool &isPaused) override;
    // void HandleVelocity(const float &timeDelta, const float &timeMultiplier, const bool &isPaused) override;
    // void Draw(const Vec2<int> &cameraPos, SDL_Renderer *renderer) override;
    //
    friend class CreatureFactory;
};

}  // namespace Creatures
