#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "./Item.hh"

class ItemFactory : public Item {
   public:
    static ItemFactory& Instance();

    std::shared_ptr<Item> CreateItem(Item::ItemID id);

    void RegisterAll();

   private:
    static ItemFactory instance;

    std::unordered_map<ItemID, std::function<std::shared_ptr<Item>()>> itemCreators;

   private:
    ItemFactory();
    ~ItemFactory();

    void RegisterItem(ItemID id, std::function<std::shared_ptr<Item>()> constructor);
};
