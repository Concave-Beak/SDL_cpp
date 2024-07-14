#pragma once

#include <memory>

#include "../../../../lib/utils/math_utils.hh"
#include "../../../main/Level.hh"
#include "../../items/Inventory.hh"
#include "../Entity.hh"

namespace Creatures {

enum class CreatureID {
    HUMAN = 0,
};

class Creature {
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

    EntityAttributes entityAttributes;
    CombatAttributes combatAttributes;
    CollisionAttributes collisionAttributes;

    Triangle<float> sightLine;
    float visionConeAngle;  // TODO make the triangle have this instead of the creature
    int visionConeRange;    // same with this

    bool isWandering = true;

    Items::Inventory inventory = Items::Inventory(0, 0);

   protected:
    void ResetVisionCone(Creature* npc);

    // Generates a randomness in a models width and height
    void GenerateModelRandomness(Vec2<int*> modelWH, Vec2<int> defaultValue, int variation);

   private:
    static void Delete(std::shared_ptr<Creature> creature);

   private:
    friend class CreatureFactory;
    friend class CreatureHandler;
};

class CreatureHandler {
    typedef std::vector<std::shared_ptr<Creature>> CreatureVector;

   public:
    ~CreatureHandler() = default;

   public:
    static std::shared_ptr<CreatureHandler> Init(SDL_Renderer* renderer);
    static void PushToCreatureEntity(std::shared_ptr<Creature> creature);
    static CreatureHandler& Instance();

    void Handle(Vec2<float> playerPos, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused);

   private:
    CreatureHandler() = default;

   private:
    static CreatureHandler handler;
    SDL_Renderer* renderer;
    static inline CreatureVector creatureVector = {};

   private:
    void HandleVelocity(Creature* creature, float timeDelta, float timeMultiplier, bool isPaused);

    void HandleVerticalCollision(Creature& creature, const LevelItem& levelItem,
                                 float timeDelta, float timeMultiplier);
    void HandleHorizontalCollision(Creature& creature, const LevelItem& levelItem,
                                   float timeDelta, float timeMultiplier);
    void HandleCollisions(Creature& creature, float timeDelta, float timeMultiplier, bool isPaused);

    void Draw(const Creature& creature, Vec2<int> cameraPos);
    void DrawSightLine(Creature& creature, Vec2<int> cameraPos, SDL_Renderer* renderer);

    void ResetCollisionState(Creature& creature);
    void UpdateModel(Creature& creature);

    bool GetCollidedInformation(const Creature& creature, Direction direction);
};

}  // namespace Creatures
