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

ItemFactory& ItemFactory::Instance() { return instance; }

void ItemFactory::RegisterItem(ItemID id, std::function<std::shared_ptr<Item>()> constructor) {
    instance.itemCreators.emplace(id, constructor);
}

std::shared_ptr<Item> ItemFactory::CreateItem(ItemID id) {
    itemFactoryMap::iterator it = itemCreators.find(id);
    if (it != itemCreators.end()) {
        return it->second();
    }
    return nullptr;
}

void ItemFactory::RegisterAll() {
    RegisterItem(SHORTSWORD, []() -> std::shared_ptr<Item> { return std::make_shared<ShortSword>(); });
    RegisterItem(BOW_AND_ARROW, []() -> std::shared_ptr<Item> { return std::make_shared<BowAndArrow>(); });
}
}  // namespace Items
