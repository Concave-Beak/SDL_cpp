#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "./Item.hh"

namespace Items {
class ItemFactory {
    typedef std::unordered_map<ItemID, std::function<std::shared_ptr<Item>()>> itemFactoryMap;

   public:
    static ItemFactory& Instance();

    std::shared_ptr<Item> CreateItem(ItemID id);

    void RegisterAll();

   private:
    static ItemFactory instance;

    itemFactoryMap itemCreators = {};

   private:
    ItemFactory();
    ~ItemFactory();

    void RegisterItem(ItemID id, std::function<std::shared_ptr<Item>()> constructor);
};
}  // namespace Items
