#pragma once
#include "Champion.h"
#include "Item.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Struct to hold the parsed data from the build file
struct ChampionBuild {
  std::string championName;
  int level;
  std::map<std::string, int> abilityRanks;
  std::vector<std::string> itemNames;
};

// The factory class is now a pure assembler
class Factory {
public:
  static Champion createChampion(const ChampionBuild &build);

private:
  static Item createItem(const std::string &itemName);
  static std::shared_ptr<Effect> createEffect(const std::string &effectName);
};
