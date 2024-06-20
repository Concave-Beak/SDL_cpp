#pragma once

#include <SDL2/SDL_stdinc.h>

#include <string>

#include "../../../lib/utils/math_utils.hh"
#include "../entities/Entity.hh"

class Item {
   public:
    enum ItemID {
        BOW_AND_ARROW = 0,
        SHORTSWORD,
    };

    void Drop(Matrix2D<Item>* inventory);
    void Attack(Entity* entityOrigin, const Vec2<float>& spawnPos, float angle);

   protected:
    Item() = default;
    ~Item() = default;

    std::string name;
    ItemID ID;

    enum ItemType {
        SHORT_BOW = 1,
        ONE_HANDED_SWORD,
    };
    ItemType itemType;

    float damage;
    float armorPenetration;
    float durability;
    float invetoryWeight;

    float range;
};
