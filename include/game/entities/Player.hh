#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include <cwchar>
#include <memory>

#include "../../../lib/utils/math_utils.hh"
#include "../../main/Level.hh"
#include "../items/Inventory.hh"
#include "../items/Item.hh"
#include "./EntityAttributes.hh"

//------------------------------------------------------------------------------

class Player {
    friend class PlayerHandler;

   public:
    ~Player() = default;

    static Vec2<float> GetRunningSpeed();
    static Vec2<float> GetWalkingSpeed();
    static Vec2<float> GetPos();
    static SDL_Rect GetModel();
    static CreatureAttributes GetAttribute();
    static const CreatureAttributes* GetAttributeReference();

   private:
    static std::shared_ptr<Player> Instance();

    enum WeaponHand {
        LEFT_HAND = 0,
        RIGHT_HAND
    };
    void SwitchWeapon(WeaponHand weaponHand);  // TODO

   private:
    static std::shared_ptr<Player> Create();

    Player() = default;
    static std::shared_ptr<Player> instance;

   private:
    CreatureAttributes creatureAttributes;

    float angleFacing = 0;

    Items::Inventory inventory = Items::Inventory(5, 5);
    std::shared_ptr<Items::Item> currentItemHolding;

   private:
    void InitInventory();
};

class PlayerHandler {
   public:
    ~PlayerHandler() = default;

   public:
    static std::shared_ptr<PlayerHandler> Init(SDL_Renderer* renderer);
    static PlayerHandler& Instance();

    void Handle(Vec2<int> mousePos_, Vec2<int> cameraPos, float timeDelta, float timeMultiplier, bool isPaused);

    void ChargeAttack();
    void ReleaseAttack();

    void MovePlayer(Direction direction, bool isPaused, bool isRunning);  // TODO: REMOVE ACCEL SPEEd

   private:
    PlayerHandler() = default;

   private:
    static PlayerHandler handler;
    std::shared_ptr<Player> playerInstance;

    SDL_Renderer* renderer = nullptr;

   private:
    void HandleVelocity(float timeDelta, float timeMultiplier, bool isPaused);

    void HandleVerticalCollision(SDL_Rect entityRect, const LevelItem& levelItem,
                                 float timeDelta, float timeMultiplier);
    void HandleHorizontalCollision(SDL_Rect entityRect, const LevelItem& levelItem,
                                   float timeDelta, float timeMultiplier);
    void HandleCollisions(float timeDelta, float timeMultiplier, bool isPaused);

    void Draw(Vec2<int> cameraPos);
    void DrawSightLine(Vec2<int> cameraPos, SDL_Renderer* renderer);

    void ResetCollisionState();
    void UpdateModel();

    bool GetCollidedInformation(Direction direction);
};

//------------------------------------------------------------------------------
