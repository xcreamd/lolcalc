#include "effects/GenericEffect.h"
#include "Champion.h"
#include <iostream>

GenericEffect::GenericEffect(
    const std::string &name, const std::vector<EffectRule> &rules,
    const std::map<std::string, nlohmann::json> &definitions)
    : name(name), rules(rules), definitions(definitions) {}

void GenericEffect::onSpellHit(Champion &owner, Champion &target) {
  processTrigger(TriggerType::OnSpellHit, owner, target);
}

void GenericEffect::onAutoAttack(Champion &owner, Champion &target) {
  processTrigger(TriggerType::OnAutoAttack, owner, target);
}

std::optional<DamageInstance> GenericEffect::getOnHitDamage(Champion &owner,
                                                            Champion &target) {
  // Special handling for OnHit to return damage immediately
  for (const auto &rule : rules) {
    if (rule.trigger == TriggerType::OnHit) {
      for (const auto &action : rule.actions) {
        if (action.type == ActionType::DealDamage) {
          float dmg = calculateScaling(action.scaling, owner, target);
          return DamageInstance{dmg, action.damageType};
        }
      }
    }
  }
  return std::nullopt;
}

void GenericEffect::processTrigger(TriggerType trigger, Champion &owner,
                                   Champion &target) {
  for (const auto &rule : rules) {
    if (rule.trigger == trigger) {
      for (const auto &action : rule.actions) {
        executeAction(action, owner, target);
      }
    }
  }
}

void GenericEffect::executeAction(const ActionConfig &action, Champion &owner,
                                  Champion &target) {
  if (action.type == ActionType::ApplyDebuff) {
    // Look up the definition in the JSON blob we saved
    if (definitions.count(action.effectName)) {
      auto def = definitions.at(action.effectName);
      std::string type = def.value("type", "");

      if (type == "DoT") {
        float damage = 0.0f;

        // Parse Scaling for the DoT from the definition
        if (def.contains("scaling")) {
          auto sc = def["scaling"];
          // Simplified Scaling Logic (Should share calculateScaling ideally)
          float maxHPRatio = sc.value("target_max_health", 0.0f);
          damage += target.getTotalStats().health * maxHPRatio;
        }

        auto dot = std::make_shared<DamageOverTimeEffect>(
            action.effectName, action.duration,
            damage, // Per tick
            def.value("tick_interval", 1.0f),
            DamageType::Magic, // Should parse string to enum in real
                               // implementation
            &owner);
        target.applyDebuff(dot);
      }
    }
  } else if (action.type == ActionType::DealDamage) {
    float damage = calculateScaling(action.scaling, owner, target);
    // Direct damage application not fully wired in event bus yet, but logic is
    // here
    std::cout << "[Generic] Dealing " << damage << " " << (int)action.damageType
              << " damage." << std::endl;
    // target.takeDamage(damage);
  }
}

float GenericEffect::calculateScaling(const ScalingData &scaling,
                                      Champion &owner, Champion &target) {
  float total = scaling.base;

  // Bonus AD Calculation
  float baseAD =
      owner.getBaseStatsData().attackDamage.base; // Simplified for now
  float bonusAD = owner.getTotalStats().attackDamage - baseAD;

  total += bonusAD * scaling.adRatio;
  total += owner.getTotalStats().abilityPower * scaling.apRatio;

  if (scaling.targetMaxHealthRatio > 0) {
    total += target.getTotalStats().health * scaling.targetMaxHealthRatio;
  }

  return total;
}
