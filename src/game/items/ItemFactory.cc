#include "../../../include/game/items/ItemFactory.hh"

#include <memory>

#include "../../../include/game/items/Weapons/Melee.hh"
#include "../../../include/game/items/Weapons/Ranged.hh"

namespace Items {
ItemFactory::ItemFactory() {
    instance.RegisterAll();
}
ItemFactory::~ItemFactory() {}

ItemFactory ItemFactory::instance = ItemFactory();

ItemFactory& ItemFactory::Instance() {
    return instance;
}

void ItemFactory::RegisterItem(Item::ItemID id, std::function<std::shared_ptr<Item>()> constructor) {
    instance.itemCreators.emplace(id, constructor);
}

std::shared_ptr<Item> ItemFactory::CreateItem(Item::ItemID id) {
    itemFactoryMap::iterator it = itemCreators.find(id);
    if (it != itemCreators.end()) {
        return it->second();
    }
    return nullptr;
}

void ItemFactory::RegisterAll() {
    RegisterItem(Item::ItemID::SHORTSWORD, []() { return std::make_shared<ShortSword>(); });
    RegisterItem(ShortSword::ItemID::BOW_AND_ARROW, []() { return std::make_shared<BowAndArrow>(); });
}
}  // namespace Items
