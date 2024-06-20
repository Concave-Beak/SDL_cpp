#include "../../../include/game/items/ItemFactory.hh"

#include <memory>

#include "../../../include/game/items/Weapons/Melee.hh"
#include "../../../include/game/items/Weapons/Ranged.hh"

ItemFactory::ItemFactory() {
    instance.RegisterAll();
}
ItemFactory::~ItemFactory() {}

ItemFactory ItemFactory::instance = ItemFactory();

ItemFactory& ItemFactory::Instance() {
    return instance;
}

void ItemFactory::RegisterItem(ItemID id, std::function<std::shared_ptr<Item>()> constructor) {
    instance.itemCreators.emplace(id, constructor);
}

std::shared_ptr<Item> ItemFactory::CreateItem(Item::ItemID id) {
    auto it = itemCreators.find(id);
    if (it != itemCreators.end()) {
        return it->second();
    }
    return nullptr;
}

void ItemFactory::RegisterAll() {
    RegisterItem(ItemID::SHORTSWORD, []() { return std::make_shared<ShortSword>(); });
    RegisterItem(ItemID::BOW_AND_ARROW, []() { return std::make_shared<BowAndArrow>(); });
}
