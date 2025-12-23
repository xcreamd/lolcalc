#pragma once
#include "Item.h"
#include <vector>

class InventoryComponent {
public:
  void addItem(const Item &item);
  const std::vector<Item> &getItems() const;

private:
  std::vector<Item> items;
};
