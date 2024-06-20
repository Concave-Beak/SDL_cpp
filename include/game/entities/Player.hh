#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include <cwchar>
#include <memory>

#include "../../../lib/utils/math_utils.hh"
#include "../items/Inventory.hh"
#include "../items/Item.hh"
#include "./Entity.hh"

//------------------------------------------------------------------------------

class Player : public Entity {
   public:
    static Player* Instance();

    Vec2<float> GetRunningSpeed();
    Vec2<float> GetWalkingSpeed();

    Vec2<int> GetHitbox();

    enum WeaponHand {
        LEFT_HAND = 0,
        RIGHT_HAND
    };
    void SwitchWeapon(WeaponHand weaponHand);
    void Attack();

    static void Handle(const Vec2<int>& mousePos);

    ~Player() = default;

   private:
    Player() = default;
    static Player* instance;

    Vec2<int> mousePos = {0, 0};
    float angleFacing = 0;

    Vec2<float> runningSpeed = {20, 125};
    Vec2<float> walkingSpeed = {10, 75};

    Inventory inventory = Inventory(5, 5);
    std::shared_ptr<Item> currentItemHolding;

   private:
    void InitInventory();

    void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) override;

    void DrawLineOfSight(const Vec2<int>& mousePos, const Vec2<int>& cameraPos, SDL_Renderer* renderer);

    void SetFacingAngle(const Vec2<int>& mousePos, const Vec2<int>& cameraPos);
    Vec2<int> GetSightLineEnd(const Vec2<int>& cameraPos, float lineLength);
};

//------------------------------------------------------------------------------
