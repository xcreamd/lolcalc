#include "DataManager.h"
#include "EffectConfig.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

static ActionConfig parseAction(const nlohmann::json &j) {
  ActionConfig action;
  std::string typeStr = j.value("type", "");

  if (typeStr == "ApplyDebuff")
    action.type = ActionType::ApplyDebuff;
  else if (typeStr == "DealDamage")
    action.type = ActionType::DealDamage;
  else if (typeStr == "ApplyBuff")
    action.type = ActionType::ApplyBuff;
  else if (typeStr == "ModifyStat")
    action.type = ActionType::ModifyStat;
  else if (typeStr == "Execute")
    action.type = ActionType::Execute;
  else if (typeStr == "DamageAmp")
    action.type = ActionType::DamageAmp;

  action.effectName = j.value("effectName", "");
  if (action.effectName.empty()) {

    action.effectName = j.value("debuff_name", "");
    if (action.effectName.empty())
      action.effectName = j.value("buff_name", "");
  }

  action.duration = j.value("duration", 0.0f);

  if (j.contains("scaling")) {
    action.scaling.base = j["scaling"].value("base", 0.0f);
    action.scaling.adRatio = j["scaling"].value("ad", 0.0f);
    action.scaling.apRatio = j["scaling"].value("ap", 0.0f);
    action.scaling.targetMaxHealthRatio =
        j["scaling"].value("target_max_health", 0.0f);
    action.scaling.targetCurrentHealthRatio =
        j["scaling"].value("target_current_health", 0.0f);
  }

  std::string dmgTypeStr = j.value("damage_type", "Physical");
  if (dmgTypeStr == "Magic")
    action.damageType = DamageType::Magic;
  else if (dmgTypeStr == "True")
    action.damageType = DamageType::True;
  else
    action.damageType = DamageType::Physical;

  action.thresholdPercent = j.value("threshold_percent", 0.0f);

  return action;
}

static std::vector<Scaling> parseScalings(const nlohmann::json &j) {
  std::vector<Scaling> scalings;

  if (j.contains("ad_ratio")) {
    float ratio = j.value("ad_ratio", 0.0f);
    if (ratio != 0.0f) {
      bool isBonus = (j.value("ad_ratio_type", "bonus") == "bonus");
      scalings.push_back({Stat::AttackDamage, ratio, isBonus});
    }
  }
  if (j.contains("ap_ratio")) {
    float ratio = j.value("ap_ratio", 0.0f);
    if (ratio != 0.0f) {
      bool isBonus = (j.value("ap_ratio_type", "total") == "bonus");
      scalings.push_back({Stat::AbilityPower, ratio, isBonus});
    }
  }

  if (j.contains("scalings") && j["scalings"].is_array()) {
    for (const auto &scaleJson : j["scalings"]) {
      Scaling s;
      s.ratio = scaleJson.value("ratio", 0.0f);
      s.isBonus = (scaleJson.value("type", "total") == "bonus");

      std::string statStr = scaleJson.value("stat", "");
      if (statStr == "AttackDamage")
        s.statType = Stat::AttackDamage;
      else if (statStr == "AbilityPower")
        s.statType = Stat::AbilityPower;
      else if (statStr == "Health")
        s.statType = Stat::Health;
      else if (statStr == "Mana")
        s.statType = Stat::Mana;
      else if (statStr == "Armor")
        s.statType = Stat::Armor;
      else if (statStr == "MagicResist")
        s.statType = Stat::MagicResist;
      else
        continue;

      scalings.push_back(s);
    }
  }
  return scalings;
}

DataManager &DataManager::getInstance() {
  static DataManager instance;
  return instance;
}

void DataManager::loadData(const std::string &dataPath) {
  std::string championPath = dataPath + "/champions";
  if (fs::exists(championPath)) {
    for (const auto &entry : fs::directory_iterator(championPath)) {
      if (entry.path().extension() == ".json") {
        try {
          std::ifstream f(entry.path());
          nlohmann::json data = nlohmann::json::parse(f);
          parseChampion(data);
        } catch (const std::exception &e) {
          std::cerr << "Error parsing champion file " << entry.path() << ": "
                    << e.what() << std::endl;
        }
      }
    }
  }
  std::cout << "Loaded " << championBlueprints.size() << " champion blueprints."
            << std::endl;

  std::string itemPath = dataPath + "/items";
  if (fs::exists(itemPath)) {
    for (const auto &entry : fs::directory_iterator(itemPath)) {
      if (entry.path().extension() == ".json") {
        try {
          std::ifstream f(entry.path());
          nlohmann::json data = nlohmann::json::parse(f);
          parseItem(data);
        } catch (const std::exception &e) {
          std::cerr << "Error parsing item file " << entry.path() << ": "
                    << e.what() << std::endl;
        }
      }
    }
  }
  std::cout << "Loaded " << itemBlueprints.size() << " item blueprints."
            << std::endl;
}

void DataManager::parseChampion(const nlohmann::json &j) {
  ChampionBlueprint bp;
  bp.name = j["name"];
  bp.strategyName = j.value("auto_attack_strategy", "Default");

  const auto &s = j["stats"];
  auto getVal = [](const nlohmann::json &node, const char *key) {
    return StatValue{node[key].value("base", 0.0f),
                     node[key].value("growth", 0.0f)};
  };

  bp.baseStats.health = getVal(s, "health");
  bp.baseStats.mana = getVal(s, "mana");
  bp.baseStats.attackDamage = getVal(s, "attack_damage");
  bp.baseStats.attackSpeedRatio = getVal(s, "attack_speed_ratio");
  bp.baseStats.armor = getVal(s, "armor");
  bp.baseStats.magicResist = getVal(s, "magic_resist");

  if (s.contains("movement_speed")) {
    if (s["movement_speed"].is_object()) {
      bp.baseStats.movementSpeed = getVal(s, "movement_speed");
    } else {
      bp.baseStats.movementSpeed = {s.value("movement_speed", 330.0f), 0.0f};
    }
  }

  if (j.contains("abilities")) {
    for (auto &[key, val] : j["abilities"].items()) {
      bp.abilities[key] = parseAbility(val);
    }
  }

  championBlueprints[bp.name] = bp;
}

ChampionBlueprint::AbilityData
DataManager::parseAbility(const nlohmann::json &j) {
  ChampionBlueprint::AbilityData ad;
  ad.name = j.value("name", "Unknown");
  ad.type = j.value("type", "");
  ad.effectName = j.value("effect_name", "");

  ad.cooldown = j.value("cooldown", std::vector<float>{});
  ad.cost = j.value("cost", std::vector<float>{});

  if (ad.type == "Skillshot") {
    if (j.contains("base_damage"))
      ad.baseDamage = j["base_damage"].get<std::vector<float>>();

    ad.scalings = parseScalings(j);

    ad.targetMaxHealthRatio = j.value("target_max_health_ratio", 0.0f);
    ad.targetCurrentHealthRatio = j.value("target_current_health_ratio", 0.0f);

    std::string dmgStr = j.value("damage_type", "Physical");
    if (dmgStr == "Magic")
      ad.damageType = DamageType::Magic;
    else if (dmgStr == "True")
      ad.damageType = DamageType::True;
    else
      ad.damageType = DamageType::Physical;
  }

  if (j.contains("passive_rules")) {
    for (const auto &ruleJson : j["passive_rules"]) {
      EffectRule rule;
      std::string trigStr = ruleJson.value("trigger", "");

      if (trigStr == "OnSpellHit")
        rule.trigger = TriggerType::OnSpellHit;
      else if (trigStr == "OnAutoAttack")
        rule.trigger = TriggerType::OnAutoAttack;
      else if (trigStr == "OnHit")
        rule.trigger = TriggerType::OnHit;
      else if (trigStr == "OnDamageTaken")
        rule.trigger = TriggerType::OnDamageTaken;
      else if (trigStr == "OnPostMitigationDamage")
        rule.trigger = TriggerType::OnPostMitigationDamage;
      else if (trigStr == "OnCrit")
        rule.trigger = TriggerType::OnCrit;
      else
        rule.trigger = TriggerType::Passive;

      for (const auto &actionJson : ruleJson["actions"]) {
        rule.actions.push_back(parseAction(actionJson));
      }
      ad.passiveRules.push_back(rule);
    }
  }

  if (j.contains("definitions")) {
    ad.definitions =
        j["definitions"].get<std::map<std::string, nlohmann::json>>();
  }

  return ad;
}

void DataManager::parseItem(const nlohmann::json &j) {
  ItemBlueprint bp;
  bp.name = j["name"];
  bp.effectName = j.value("effect_name", "");

  if (j.contains("stats")) {
    for (auto &[key, val] : j["stats"].items()) {
      if (key == "AttackDamage")
        bp.stats[Stat::AttackDamage] = val;
      else if (key == "AbilityPower")
        bp.stats[Stat::AbilityPower] = val;
      else if (key == "Armor")
        bp.stats[Stat::Armor] = val;
      else if (key == "MagicResist")
        bp.stats[Stat::MagicResist] = val;
      else if (key == "Health")
        bp.stats[Stat::Health] = val;
      else if (key == "Mana")
        bp.stats[Stat::Mana] = val;
      else if (key == "AttackSpeed")
        bp.stats[Stat::AttackSpeed] = val;
      else if (key == "AbilityHaste")
        bp.stats[Stat::AbilityHaste] = val;
      else if (key == "CriticalStrikeChance")
        bp.stats[Stat::CriticalStrikeChance] = val;
      else if (key == "MovementSpeed")
        bp.stats[Stat::MovementSpeed] = val;
      else if (key == "LifeSteal") {
      }
    }
  }

  if (j.contains("passive_rules")) {
    for (const auto &ruleJson : j["passive_rules"]) {
      EffectRule rule;
      std::string trigStr = ruleJson.value("trigger", "");

      if (trigStr == "OnSpellHit")
        rule.trigger = TriggerType::OnSpellHit;
      else if (trigStr == "OnAutoAttack")
        rule.trigger = TriggerType::OnAutoAttack;
      else if (trigStr == "OnHit")
        rule.trigger = TriggerType::OnHit;
      else if (trigStr == "OnDamageTaken")
        rule.trigger = TriggerType::OnDamageTaken;
      else if (trigStr == "OnPostMitigationDamage")
        rule.trigger = TriggerType::OnPostMitigationDamage;
      else if (trigStr == "OnCrit")
        rule.trigger = TriggerType::OnCrit;
      else
        rule.trigger = TriggerType::Passive;

      for (const auto &actionJson : ruleJson["actions"]) {
        rule.actions.push_back(parseAction(actionJson));
      }
      bp.passiveRules.push_back(rule);
    }
  }

  if (j.contains("definitions")) {
    bp.definitions =
        j["definitions"].get<std::map<std::string, nlohmann::json>>();
  }

  itemBlueprints[bp.name] = bp;
}

const ChampionBlueprint *
DataManager::getChampionBlueprint(const std::string &name) const {
  auto it = championBlueprints.find(name);
  return (it != championBlueprints.end()) ? &it->second : nullptr;
}

const ItemBlueprint *
DataManager::getItemBlueprint(const std::string &name) const {
  auto it = itemBlueprints.find(name);
  return (it != itemBlueprints.end()) ? &it->second : nullptr;
}

const nlohmann::json &
DataManager::getChampionData(const std::string &name) const {

  static nlohmann::json empty;
  return empty;
}
