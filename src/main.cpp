#include "DataManager.h"
#include "Factory.h"
#include "json.hpp"
#include "strategies/DefaultAutoAttackStrategy.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// Helper to parse build files
ChampionBuild createBuildFromJSON(const nlohmann::json &data,
                                  const std::string &filename) {
  ChampionBuild build;
  try {
    build.championName = data.at("champion").get<std::string>();
    build.level = data.at("level").get<int>();
    build.abilityRanks =
        data.at("ability_ranks").get<std::map<std::string, int>>();
    build.itemNames = data.at("items").get<std::vector<std::string>>();
  } catch (const nlohmann::json::exception &e) {
    std::cerr << "Error processing build from " << filename << ": " << e.what()
              << std::endl;
    exit(1);
  }
  return build;
}

void runSustainTest(const std::string &dataPath) {
  std::cout << "\n=== RUNNING SUSTAIN (LIFE STEAL) TEST ===" << std::endl;
  DataManager::getInstance().loadData(dataPath);

  // 1. Create Draven with Bloodthirster
  // Bloodthirster should provide ~18% Life Steal and 80 AD based on previous
  // data updates
  ChampionBuild build;
  build.championName = "Draven";
  build.level = 15;
  build.abilityRanks = {{"Q", 5}, {"W", 5}, {"E", 5}, {"R", 3}};
  build.itemNames = {"Bloodthirster"};

  Champion draven = Factory::createChampion(build);

  // 2. Create Dummy Target (Low Armor to maximize damage/healing visibility)
  BaseStats dummyStats;
  dummyStats.health = {3000, 0};
  dummyStats.armor = {0, 0};
  std::unique_ptr<AutoAttackStrategy> dummyStrategy =
      std::make_unique<DefaultAutoAttackStrategy>();
  Champion dummy("Dummy", dummyStats, std::move(dummyStrategy));

  // 3. Setup Health State
  float maxHP = draven.getTotalStats().health;
  draven.takeDamage(maxHP * 0.5f); // Reduce Draven to 50% HP

  std::cout << "\n[Setup]" << std::endl;
  std::cout << "Draven Max HP: " << maxHP << std::endl;
  std::cout << "Draven Current HP: " << draven.getCurrentHealth() << std::endl;
  std::cout << "Attack Damage: " << draven.getTotalStats().attackDamage
            << std::endl;
  std::cout << "Life Steal: " << (draven.getTotalStats().lifeSteal * 100.0f)
            << "%" << std::endl;

  // 4. Perform Attack
  std::cout << "\n[Action] Attacking Dummy..." << std::endl;
  draven.performAutoAttack(dummy);

  // 5. Verify Result
  std::cout << "\n[Result]" << std::endl;
  std::cout << "Draven HP After: " << draven.getCurrentHealth() << std::endl;

  float healedAmount = draven.getCurrentHealth() - (maxHP * 0.5f);
  std::cout << "Total Healed: " << healedAmount << std::endl;

  std::cout << "=== SUSTAIN TEST COMPLETE ===\n" << std::endl;
}

int main(int argc, char *argv[]) {
  // Determine data path based on executable location
  std::filesystem::path exePath = std::filesystem::path(argv[0]);
  std::filesystem::path dataPath = exePath.parent_path() / ".." / "data";

  // Run the Sustain Test
  runSustainTest(dataPath.string());

  // Optional: Standard Simulation Mode
  if (argc == 3) {
    DataManager::getInstance().loadData(dataPath.string());

    std::ifstream f1(argv[1]);
    nlohmann::json data1 = nlohmann::json::parse(f1);
    std::ifstream f2(argv[2]);
    nlohmann::json data2 = nlohmann::json::parse(f2);

    ChampionBuild build1 = createBuildFromJSON(data1, argv[1]);
    ChampionBuild build2 = createBuildFromJSON(data2, argv[2]);

    Champion c1 = Factory::createChampion(build1);
    Champion c2 = Factory::createChampion(build2);

    std::cout << "\n--- Full Simulation: " << c1.getName() << " vs "
              << c2.getName() << " ---" << std::endl;
    c1.performAutoAttack(c2);
  }

  return 0;
}
