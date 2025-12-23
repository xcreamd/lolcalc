#pragma once
#include "Effect.h"
#include "Enums.h"
#include "Stats.h"
#include <memory>
#include <string>
#include <vector>

class Champion; // Forward declaration

// Defines how a specific part of damage scales (e.g. 0.5 AD or 0.04 Bonus Mana)
struct Scaling {
  Stat statType; // e.g. Stat::AttackDamage, Stat::Mana
  float ratio;   // e.g. 1.0 (100%), 0.04 (4%)
  bool isBonus;  // true = scale off Bonus stats only, false = Total
};

struct DamagePart {
  std::vector<float> baseDamage;
  std::vector<Scaling> scalings; // Replaces fixed adRatio/apRatio

  // Target scaling remains separate as it depends on the target, not owner
  float targetMaxHealthRatio = 0.0f;
  float targetCurrentHealthRatio = 0.0f;
};

struct Condition {
  std::string requiredStatus;
  float damageMultiplier = 1.0f;
};

class Ability {
public:
  // Constructor for damage abilities (Skillshots) with Cooldowns and Costs
  Ability(const std::string &name, const std::vector<DamagePart> &parts,
          DamageType type, const std::vector<float> &cooldowns,
          const std::vector<float> &costs,
          const std::vector<Condition> &conditions = {},
          const std::string &status = "", float statusDuration = 0.0f);

  // Constructor for effect-only abilities (Buffs/Utility) with Cooldowns and
  // Costs
  Ability(const std::string &name, std::shared_ptr<Effect> effect,
          const std::vector<float> &cooldowns = {},
          const std::vector<float> &costs = {});

  // Calculates raw damage before mitigation
  float calculateDamage(int rank, const FinalStats &totalStats,
                        const BaseStats &baseStats, Champion &owner,
                        Champion &target) const;

  // Core Logic for Time & Resources
  bool canCast(const FinalStats &ownerStats, float currentMana, int rank) const;
  void putOnCooldown(const FinalStats &ownerStats, int rank);
  void reduceCooldown(float amount);
  void tick(float deltaTime);
  void resetCooldown();

  // Getters
  std::string getName() const;
  DamageType getDamageType() const;
  std::shared_ptr<Effect> getEffect() const;
  std::string getStatusToApply() const;
  float getStatusDuration() const;

  float getResourceCost(int rank) const;
  float getCurrentCooldown() const;

  void activate();

private:
  std::string name;
  DamageType damageType = DamageType::Physical;
  std::vector<DamagePart> damageParts;
  std::vector<Condition> conditions;

  // Cooldown and Resource Data
  std::vector<float> baseCooldowns; // Per rank
  std::vector<float> baseCosts;     // Per rank
  float currentCooldown = 0.0f;

  std::string statusToApply;
  float statusDuration = 0.0f;
  std::shared_ptr<Effect> effect = nullptr;
};
