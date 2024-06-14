#pragma once

#include <string>
struct Inventory;

#include <SDL2/SDL_stdinc.h>

#include "../../../lib/utils/math_utils.hh"
#include "../entities/Entity.hh"

class Item {
   public:
    enum ItemID {
        BOW_AND_ARROW = 0,
        SHORTSWORD,
    };
    Item(ItemID itemID_);
    ~Item();

    void Drop(Matrix2D<Item>* inventory);
    void Attack(Entity* entityOrigin, const Vec2<float>& spawnPos, float angle);

   private:
    ItemID ID;
    std::string name;

    enum ItemType {
        SHORT_BOW = 1,
        ONE_HANDED_SWORD,
    };
    ItemType itemTtype;

    float damage;
    float armorPenetration;
    float durability;
    float invetoryWeight;

    float range;
};
