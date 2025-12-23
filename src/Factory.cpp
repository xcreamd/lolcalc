#include "Factory.h"
#include "DataManager.h"
#include "EffectConfig.h"
#include "EffectRegistry.h"
#include "effects/GenericEffect.h"
#include "effects/champion/ashe/AsheQEffect.h"
#include "effects/champion/draven/DravenQEffect.h"
#include "effects/champion/draven/DravenWEffect.h"
#include "effects/champion/jhin/JhinPassiveEffect.h"
#include "effects/champion/ryze/RyzePEffect.h"
#include "strategies/DefaultAutoAttackStrategy.h"
#include "strategies/ashe/AsheAutoAttackStrategy.h"
#include "strategies/jhin/JhinAutoAttackStrategy.h"
#include <iostream>

static AbilitySlot stringToSlot(const std::string &str) {
  if (str == "Q")
    return AbilitySlot::Q;
  if (str == "W")
    return AbilitySlot::W;
  if (str == "E")
    return AbilitySlot::E;
  if (str == "R")
    return AbilitySlot::R;
  if (str == "P" || str == "Passive")
    return AbilitySlot::Passive;
  return AbilitySlot::Unknown;
}

ActionConfig parseAction(const nlohmann::json &j) {
  ActionConfig action;
  std::string typeStr = j.value("type", "");

  if (typeStr == "ApplyDebuff")
    action.type = ActionType::ApplyDebuff;
  else if (typeStr == "DealDamage")
    action.type = ActionType::DealDamage;
  else if (typeStr == "ApplyBuff")
    action.type = ActionType::ApplyBuff;
  else if (typeStr == "SwapForm")
    action.type = ActionType::SwapForm;
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

  action.formName = j.value("form", "");
  action.duration = j.value("duration", 0.0f);
  action.thresholdPercent = j.value("threshold_percent", 0.0f);

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

  return action;
}

std::vector<Scaling> parseScalings(const nlohmann::json &j) {
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
      else if (statStr == "LifeSteal")
        s.statType = Stat::LifeSteal;
      else if (statStr == "OmniVamp")
        s.statType = Stat::OmniVamp;
      else
        continue;

      scalings.push_back(s);
    }
  }
  return scalings;
}

std::shared_ptr<Effect> createEffectFromJSON(const std::string &name,
                                             const nlohmann::json &json) {
  std::vector<EffectRule> rules;

  if (json.contains("passive_rules")) {
    for (const auto &ruleJson : json["passive_rules"]) {
      EffectRule rule;
      std::string trigStr = ruleJson.value("trigger", "");

      if (trigStr == "OnSpellHit")
        rule.trigger = TriggerType::OnSpellHit;
      else if (trigStr == "OnAutoAttack")
        rule.trigger = TriggerType::OnAutoAttack;
      else if (trigStr == "OnHit")
        rule.trigger = TriggerType::OnHit;
      else if (trigStr == "OnCast")
        rule.trigger = TriggerType::OnCast;
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
      rules.push_back(rule);
    }
  }

  std::map<std::string, nlohmann::json> definitions;
  if (json.contains("definitions")) {
    definitions =
        json["definitions"].get<std::map<std::string, nlohmann::json>>();
  }

  return std::make_shared<GenericEffect>(name, rules, definitions);
}

Ability createAbilityFromJSON(const std::string &name,
                              const nlohmann::json &abilityJson) {
  std::string type = abilityJson.value("type", "");
  std::vector<float> cooldowns =
      abilityJson.value("cooldown", std::vector<float>{});
  std::vector<float> costs = abilityJson.value("cost", std::vector<float>{});

  if (type == "Effect" || type == "TimedEffect") {
    std::shared_ptr<Effect> effect = nullptr;
    if (abilityJson.contains("passive_rules")) {
      effect = createEffectFromJSON(name, abilityJson);
    } else {
      std::string effectName = abilityJson.value("effect_name", "");
      if (!effectName.empty()) {
        effect = EffectRegistry::getInstance().createEffect(effectName);
        if (!effect) { /* Fallbacks handled in Factory::createEffect if needed
                        */
        }
      }
    }
    return Ability(name, effect, cooldowns, costs);
  } else if (type == "Skillshot") {
    DamagePart part;
    if (abilityJson.contains("base_damage"))
      part.baseDamage = abilityJson["base_damage"].get<std::vector<float>>();
    else
      part.baseDamage = {0.0f};
    part.scalings = parseScalings(abilityJson);
    part.targetMaxHealthRatio =
        abilityJson.value("target_max_health_ratio", 0.0f);
    part.targetCurrentHealthRatio =
        abilityJson.value("target_current_health_ratio", 0.0f);

    std::string dmgTypeStr = abilityJson.value("damage_type", "Physical");
    DamageType dmgType = DamageType::Physical;
    if (dmgTypeStr == "Magic")
      dmgType = DamageType::Magic;
    else if (dmgTypeStr == "True")
      dmgType = DamageType::True;

    return Ability(name, {part}, dmgType, cooldowns, costs);
  }
  return Ability("Unknown", nullptr);
}

Champion Factory::createChampion(const ChampionBuild &build) {
  const auto *bp =
      DataManager::getInstance().getChampionBlueprint(build.championName);
  if (!bp) {
    throw std::runtime_error("Champion blueprint not found for: " +
                             build.championName);
  }

  std::unique_ptr<AutoAttackStrategy> strategy;
  if (bp->strategyName == "Jhin")
    strategy = std::make_unique<JhinAutoAttackStrategy>();
  else if (bp->strategyName == "Ashe")
    strategy = std::make_unique<AsheAutoAttackStrategy>();
  else
    strategy = std::make_unique<DefaultAutoAttackStrategy>();

  Champion champion(build.championName, bp->baseStats, std::move(strategy));
  champion.setLevel(build.level);

  for (const auto &[keyStr, data] : bp->abilities) {
    AbilitySlot slot = stringToSlot(keyStr);
    if (slot == AbilitySlot::Unknown)
      continue;

    if (data.type == "Effect" || data.type == "TimedEffect") {
      std::shared_ptr<Effect> effect = nullptr;

      if (!data.passiveRules.empty()) {
        effect = std::make_shared<GenericEffect>(data.name, data.passiveRules,
                                                 data.definitions);
      } else if (!data.effectName.empty()) {
        effect = EffectRegistry::getInstance().createEffect(data.effectName);
        if (!effect) {

          if (data.effectName == "DravenQEffect")
            effect = std::make_shared<DravenQEffect>();
          else if (data.effectName == "DravenWEffect")
            effect = std::make_shared<DravenWEffect>();
          else if (data.effectName == "AsheQEffect")
            effect = std::make_shared<AsheQEffect>();
          else if (data.effectName == "JhinPassiveEffect")
            effect = std::make_shared<JhinPassiveEffect>();
          else if (data.effectName == "RyzePEffect")
            effect = std::make_shared<RyzePEffect>();
        }
      }
      champion.addAbility("Default", slot,
                          Ability(data.name, effect, data.cooldown, data.cost));
    } else if (data.type == "Skillshot") {
      DamagePart part;
      part.baseDamage = data.baseDamage;
      part.scalings = data.scalings;
      part.targetMaxHealthRatio = data.targetMaxHealthRatio;
      part.targetCurrentHealthRatio = data.targetCurrentHealthRatio;

      champion.addAbility("Default", slot,
                          Ability(data.name, {part}, data.damageType,
                                  data.cooldown, data.cost));
    }
  }

  std::map<AbilitySlot, int> enumRanks;
  for (const auto &[keyStr, rank] : build.abilityRanks) {
    AbilitySlot slot = stringToSlot(keyStr);
    if (slot != AbilitySlot::Unknown) {
      enumRanks[slot] = rank;
    }
  }
  champion.setAbilityRanks(enumRanks);

  for (const auto &itemName : build.itemNames) {
    champion.addItem(createItem(itemName));
  }

  return champion;
}

Item Factory::createItem(const std::string &itemName) {
  const auto *bp = DataManager::getInstance().getItemBlueprint(itemName);
  if (!bp) {
    std::cerr << "Warning: Item blueprint not found for: " << itemName
              << std::endl;
    return Item(itemName, {}, nullptr);
  }

  std::shared_ptr<Effect> effect = nullptr;

  if (!bp->passiveRules.empty()) {
    effect = std::make_shared<GenericEffect>(itemName, bp->passiveRules,
                                             bp->definitions);
  } else if (!bp->effectName.empty()) {
    effect = EffectRegistry::getInstance().createEffect(bp->effectName);
  }

  return Item(itemName, bp->stats, effect);
}

std::unique_ptr<Effect> Factory::createEffect(const std::string &effectName) {
  return nullptr;
}
