#pragma once
#include "Blueprints.h" // Include the blueprints
#include "json.hpp"
#include <map>
#include <string>

class DataManager {
public:
  static DataManager &getInstance();
  void loadData(const std::string &dataPath);

  const ChampionBlueprint *getChampionBlueprint(const std::string &name) const;
  const ItemBlueprint *getItemBlueprint(const std::string &name) const;

  const nlohmann::json &getChampionData(const std::string &name) const;

private:
  DataManager() = default;

  std::map<std::string, ChampionBlueprint> championBlueprints;
  std::map<std::string, ItemBlueprint> itemBlueprints;

  std::map<std::string, nlohmann::json> championData;
  nlohmann::json notFoundJson;

  void parseChampion(const nlohmann::json &json);
  void parseItem(const nlohmann::json &json);

  ChampionBlueprint::AbilityData parseAbility(const nlohmann::json &json);
};
