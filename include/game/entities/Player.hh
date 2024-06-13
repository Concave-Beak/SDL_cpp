#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "../../../lib/utils/math_utils.hh"
#include "../items/Item.hh"
#include "./Entity.hh"

//------------------------------------------------------------------------------

class Player : public Entity {
   public:
    static Player* GetPlayerInstace();

    Vec2<float> GetPos();

    Vec2<float> GetVelocityNow();
    Vec2<float> GetRunningSpeed();
    Vec2<float> GetWalkingSpeed();

    Vec2<int> GetHitbox();

    enum WeaponHand {
        LEFT_HAND = 0,
        RIGHT_HAND
    };
    void SwitchWeapon(WeaponHand weaponHand);
    void Attack();

   private:
    static Player* playerInstance;

    Vec2<float> runningSpeed = {1, 200};
    Vec2<float> walkingSpeed = {0.5, 150};  // TODO: make private

    Vec2<Uint8> inventorySize;
    Matrix2D<Item> inventory = Matrix2D<Item>{
        {
            Item(Item::SHORTSWORD),
            Item(Item::BOW_AND_ARROW),
        },
    };
    Item currentItemHolding = inventory[0][0];

   private:
    void Draw(const Vec2<int>& cameraPos, SDL_Renderer* renderer) override;
};

//------------------------------------------------------------------------------
