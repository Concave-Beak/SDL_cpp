#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "./Item.hh"

namespace Items {
class ItemFactory {
    using itemFactoryMap = std::unordered_map<Item::ItemID, std::function<std::shared_ptr<Item>()>>;

   public:
    static ItemFactory& Instance();

    std::shared_ptr<Item> CreateItem(Item::ItemID id);

    void RegisterAll();

   private:
    static ItemFactory instance;

    itemFactoryMap itemCreators;

   private:
    ItemFactory();
    ~ItemFactory();

    void RegisterItem(Item::ItemID id, std::function<std::shared_ptr<Item>()> constructor);
};
}  // namespace Items
