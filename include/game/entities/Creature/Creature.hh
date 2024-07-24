#pragma once

#include <SDL2/SDL_render.h>

#include <cstddef>
#include <memory>

#include "../../../../lib/utils/math_utils.hh"
#include "../EntityAttributes.hh"
#include "./Components/Component.hh"

namespace Creatures {

void GenerateModelRandomness(Vec2<int*> modelWH, Vec2<int> defaultValue, int variation);

class Creature {
    friend class CreatureHandler;
    friend class Components::DrawingComponent;  // Placeholder

   public:
    bool GetCollidedInformation(Direction direction);

    virtual ~Creature() = default;

    Uint32 GetID();

    CreatureType GetCreatureType();  // TODO

    EntityAttributes GetAttributeCopy();
    const EntityAttributes* GetAttributeConstReference();

    void UpdateModel();
    void ResetCollisionState();

    void Damage(int damage);

   public:
    Creature(std::unique_ptr<Components::AIComponent> aiComponent_);

    CreatureAttributes creatureAttributes;
    EntityAttributes entityAttributes;

    // TODO make it as a field in other classes
    Triangle<float> sightLine;
    float visionConeAngle;  // TODO make the triangle have this instead of the creature
    int visionConeRange;    // same with this

   private:
    std::unique_ptr<Components::AIComponent> aiComponent = nullptr;
    std::unique_ptr<Components::DrawingComponent> drawingComponent = nullptr;
    std::unique_ptr<Components::PhysicsComponent> physicsComponent = nullptr;
    std::unique_ptr<Components::CollisionComponent> collisionComponent = nullptr;

    bool isMarkedForDeletion = false;
};

class CreatureHandler {
    typedef std::vector<std::shared_ptr<Creature>> CreatureVector;

   public:
    ~CreatureHandler() = default;

   public:
    static void PushToCreatureVector(std::shared_ptr<Creature> creature);

    static const CreatureVector& GetCreatureVector();

    static void InvokeHandler(Vec2<int> cameraPos, SDL_Renderer* renderer, float timeDelta, float timeMultiplier, bool isPaused);

   private:
    CreatureHandler() = default;

   private:
    static CreatureVector creatureVector;

   private:
    static void CheckDeletion();
    static void Delete(std::shared_ptr<Creature> creature);

    static void DrawSightLine(const std::shared_ptr<Creature> creature, Vec2<int> cameraPos, SDL_Renderer* renderer);

    static void ResetVisionCone(std::shared_ptr<Creature> creature);
};

/*
class Humanoid : public Creature {
    friend class HumanoidHandler;

   public:
    virtual ~Humanoid() = default;

   protected:
    Humanoid() = default;
};
*/

/*
class HumanoidHandler {
   public:
    static void Handler(std::shared_ptr<Creature> humanoid, Vec2<float> cameraPos, float timeDelta, float timeMultiplier, SDL_Renderer* renderer);

   private:
    static HumanoidHandler instanceHandler;

    void HandleVelocity(Humanoid* humanoid, float timeDelta, float timeMultiplier);

    void HandleVerticalCollision(Humanoid* humanoid, const LevelItem& levelItem, float timeDelta, float timeMultiplier);
    void HandleHorizontalCollision(Humanoid* humanoid, const LevelItem& levelItem, float timeDelta, float timeMultiplier);
    void HandleCollisions(Humanoid* humanoid, float timeDelta, float timeMultiplier);

    void HandleAggro(Humanoid* humanoid);

    void Draw(const Humanoid* humanoid, Vec2<int> cameraPos, SDL_Renderer* renderer);

    void Move(Humanoid* humanoid);
};
*/

}  // namespace Creatures
