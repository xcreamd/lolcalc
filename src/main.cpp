#include "DataManager.h"
#include "Enums.h"
#include "Factory.h"
#include "json.hpp"
#include "strategies/DefaultAutoAttackStrategy.h" // Needed for dummy
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// Helper to parse build files (used for general simulation)
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

// --- JHIN MECHANICS TEST ---
void runJhinTest(const std::string &dataPath) {
  std::cout << "\n=== RUNNING JHIN MECHANICS TEST ===" << std::endl;
  DataManager::getInstance().loadData(dataPath);

  // 1. Create Jhin Build
  ChampionBuild build;
  build.championName = "Jhin";
  build.level = 18;
  build.abilityRanks = {{"Q", 5}, {"W", 5}, {"E", 5}, {"R", 3}};

  // We give him items to test his Passive conversion logic:
  // - Infinity Edge: 65 AD, 25% Crit
  // - Berserker's Greaves: 35% Attack Speed (Should convert to AD on Jhin)
  // - Bloodthirster: 55 AD, 20% Crit
  build.itemNames = {"Infinity Edge", "Berserker's Greaves", "Bloodthirster"};

  Champion jhin = Factory::createChampion(build);

  // 2. Create Dummy Target
  BaseStats dummyStats;
  dummyStats.health = {2500, 0};
  dummyStats.armor = {50, 0};
  std::unique_ptr<AutoAttackStrategy> dummyStrategy =
      std::make_unique<DefaultAutoAttackStrategy>();
  Champion dummy("Dummy", dummyStats, std::move(dummyStrategy));

  // --- TEST 1: PASSIVE SCALING (Whisper) ---
  std::cout << "\n[Test 1] Jhin Passive Stats (Attack Speed -> AD Conversion)"
            << std::endl;
  // Expected behavior:
  // - Attack Speed should remain near base (0.625) despite Greaves.
  // - Attack Damage should be massive (Base + Items + % Bonus from AS/Crit
  // conversion).
  // - Crit Damage should be reduced (Base 175% + 40% IE - 25% Jhin Passive =
  // 190%).
  jhin.printStats();

  // --- TEST 2: AMMO SYSTEM & 4TH SHOT EXECUTE ---
  std::cout << "\n[Test 2] 4-Shot Mechanism & Reload" << std::endl;

  // Shot 1
  std::cout << "1. Attacking..." << std::endl;
  jhin.performAutoAttack(dummy);

  // Shot 2
  std::cout << "2. Attacking..." << std::endl;
  jhin.performAutoAttack(dummy);

  // Shot 3
  std::cout << "3. Attacking..." << std::endl;
  jhin.performAutoAttack(dummy);

  // Shot 4 (Guaranteed Crit + Execute)
  std::cout << "4. Attacking (Expect 4th Shot Crit + Execute)..." << std::endl;
  float hpBefore = dummy.getCurrentHealth();
  jhin.performAutoAttack(dummy);
  float hpAfter = dummy.getCurrentHealth();
  std::cout << "   -> Damage dealt: " << (hpBefore - hpAfter) << std::endl;

  // Shot 5 (Reload)
  std::cout << "5. Attacking (Expect Reload / No Damage)..." << std::endl;
  jhin.performAutoAttack(dummy);

  std::cout << "Simulating 2.0s wait..." << std::endl;
  jhin.tick(2.0f);

  std::cout << "6. Attacking (Should STILL FAIL - only 2.0s passed)..."
            << std::endl;
  jhin.performAutoAttack(dummy);

  std::cout << "Simulating 1.0s wait (Total 3.0s)..." << std::endl;
  jhin.tick(1.0f);

  std::cout << "7. Attacking (Should SUCCEED - Reload Complete)..."
            << std::endl;
  jhin.performAutoAttack(dummy); // 1 (Next Mag)

  // --- TEST 3: ABILITY CASTING ---
  std::cout << "\n[Test 3] Casting Q (Dancing Grenade) with Massive AD"
            << std::endl;
  jhin.castAbility(AbilitySlot::Q, dummy);

  std::cout << "=== JHIN TEST COMPLETE ===\n" << std::endl;
}

int main(int argc, char *argv[]) {
  std::filesystem::path exePath = std::filesystem::path(argv[0]);
  std::filesystem::path dataPath = exePath.parent_path() / ".." / "data";

  // Run the specific Jhin test
  runJhinTest(dataPath.string());

  // Optional: Standard Simulation Mode if arguments are provided
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
