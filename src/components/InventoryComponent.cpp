#include "InventoryComponent.h"

void InventoryComponent::addItem(const Item &item) { items.push_back(item); }

const std::vector<Item> &InventoryComponent::getItems() const { return items; }
