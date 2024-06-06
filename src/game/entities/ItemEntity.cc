#include "../../../include/game/entities/ItemEntity.hh"

ItemEntity::ItemEntity(Item item_) : item(item_), entity(Entity::ITEM_ENTITY, {0, 0}, {10, 10}) {}

void ItemEntity::PickUp() {}
