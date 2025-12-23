#pragma once
#include "Ability.h"
#include "EffectConfig.h"
#include "Enums.h"
#include "Stats.h"
#include <map>
#include <string>
#include <vector>

struct ChampionBlueprint {
  std::string name;
  BaseStats baseStats;
  std::string strategyName;

  struct AbilityData {
    std::string name;
    std::string type;
    std::string effectName;

    std::vector<float> baseDamage;
    std::vector<float> cooldown;
    std::vector<float> cost;
    std::vector<Scaling> scalings;

    float targetMaxHealthRatio = 0.0f;
    float targetCurrentHealthRatio = 0.0f;
    DamageType damageType = DamageType::Physical;

    std::vector<EffectRule> passiveRules;
    std::map<std::string, nlohmann::json> definitions;
  };

  std::map<std::string, AbilityData> abilities;
};

struct ItemBlueprint {
  std::string name;
  std::map<Stat, float> stats;
  std::string effectName;

  std::vector<EffectRule> passiveRules;
  std::map<std::string, nlohmann::json> definitions;
};
