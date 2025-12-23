#pragma once
#include "Effect.h"
#include "EffectConfig.h"
#include <map>
#include <vector>

class GenericEffect : public Effect {
private:
  std::string name;
  std::vector<EffectRule> rules;
  std::map<std::string, nlohmann::json>
      definitions; // Stores details for buffs/dots

public:
  GenericEffect(const std::string &name, const std::vector<EffectRule> &rules,
                const std::map<std::string, nlohmann::json> &definitions);

  // Overrides from base Effect class to hook into the engine
  void onSpellHit(Champion &owner, Champion &target) override;
  void onAutoAttack(Champion &owner, Champion &target) override;
  std::optional<DamageInstance> getOnHitDamage(Champion &owner,
                                               Champion &target) override;

private:
  // The core logic engine
  void processTrigger(TriggerType trigger, Champion &owner, Champion &target);
  void executeAction(const ActionConfig &action, Champion &owner,
                     Champion &target);

  // Helpers
  float calculateScaling(const ScalingData &scaling, Champion &owner,
                         Champion &target);
};
