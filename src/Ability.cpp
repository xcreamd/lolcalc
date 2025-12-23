#include "Ability.h"
#include "Champion.h"
#include <cmath> // for std::max
#include <iostream>

// Constructor for effect-only abilities
Ability::Ability(const std::string &name, std::shared_ptr<Effect> effect,
                 const std::vector<float> &cooldowns,
                 const std::vector<float> &costs)
    : name(name), effect(effect), baseCooldowns(cooldowns), baseCosts(costs) {}

// Constructor for damage/skillshot abilities
Ability::Ability(const std::string &name, const std::vector<DamagePart> &parts,
                 DamageType type, const std::vector<float> &cooldowns,
                 const std::vector<float> &costs,
                 const std::vector<Condition> &conditions,
                 const std::string &status, float statusDuration)
    : name(name), damageParts(parts), damageType(type),
      baseCooldowns(cooldowns), baseCosts(costs), conditions(conditions),
      statusToApply(status), statusDuration(statusDuration) {}

// Helper: Calculate Base Stat at a specific level
static float getBaseStatValue(const BaseStats &base, Stat stat, int level) {
  if (level <= 1) {
    switch (stat) {
    case Stat::Health:
      return base.health.base;
    case Stat::Mana:
      return base.mana.base;
    case Stat::AttackDamage:
      return base.attackDamage.base;
    case Stat::Armor:
      return base.armor.base;
    case Stat::MagicResist:
      return base.magicResist.base;
    case Stat::AttackSpeed:
      return base.attackSpeedRatio.base;
    default:
      return 0.0f;
    }
  }

  float n = static_cast<float>(level);
  auto grow = [&](float b, float g) {
    return b + g * (n - 1) * (0.7025f + 0.0175f * (n - 1));
  };

  switch (stat) {
  case Stat::Health:
    return grow(base.health.base, base.health.growth);
  case Stat::Mana:
    return grow(base.mana.base, base.mana.growth);
  case Stat::AttackDamage:
    return grow(base.attackDamage.base, base.attackDamage.growth);
  case Stat::Armor:
    return grow(base.armor.base, base.armor.growth);
  case Stat::MagicResist:
    return grow(base.magicResist.base, base.magicResist.growth);
  case Stat::AttackSpeed:
    return base.attackSpeedRatio.base;
  default:
    return 0.0f;
  }
}

static float getTotalStatValue(const FinalStats &stats, Stat stat) {
  switch (stat) {
  case Stat::Health:
    return stats.health;
  case Stat::Mana:
    return stats.mana;
  case Stat::AttackDamage:
    return stats.attackDamage;
  case Stat::AbilityPower:
    return stats.abilityPower;
  case Stat::Armor:
    return stats.armor;
  case Stat::MagicResist:
    return stats.magicResist;
  case Stat::AttackSpeed:
    return stats.attackSpeed;
  case Stat::CriticalStrikeChance:
    return stats.criticalStrikeChance;
  case Stat::Lethality:
    return stats.lethality;
  default:
    return 0.0f;
  }
}

float Ability::calculateDamage(int rank, const FinalStats &totalStats,
                               const BaseStats &baseStats, Champion &owner,
                               Champion &target) const {
  float totalRawDamage = 0;
  int index = rank - 1;

  for (const auto &part : damageParts) {
    float base = 0.0f;
    if (index >= 0 && index < part.baseDamage.size()) {
      base = part.baseDamage[index];
    }
    totalRawDamage += base;

    for (const auto &scale : part.scalings) {
      float valueToScale = 0.0f;
      float totalVal = getTotalStatValue(totalStats, scale.statType);

      if (scale.isBonus) {
        float baseVal =
            getBaseStatValue(baseStats, scale.statType, owner.getLevel());
        valueToScale = totalVal - baseVal;
        if (valueToScale < 0)
          valueToScale = 0;
      } else {
        valueToScale = totalVal;
      }

      totalRawDamage += valueToScale * scale.ratio;
    }

    if (part.targetMaxHealthRatio > 0.0f) {
      totalRawDamage +=
          target.getTotalStats().health * part.targetMaxHealthRatio;
    }
    if (part.targetCurrentHealthRatio > 0.0f) {
      totalRawDamage +=
          target.getCurrentHealth() * part.targetCurrentHealthRatio;
    }
  }

  for (const auto &condition : conditions) {
    if (target.hasStatus(condition.requiredStatus)) {
      std::cout << "[Condition] Target is " << condition.requiredStatus
                << "! Applying damage multiplier." << std::endl;
      totalRawDamage *= condition.damageMultiplier;
    }
  }

  return totalRawDamage;
}

// --- CORE LOGIC: Time & Resources ---

bool Ability::canCast(const FinalStats &ownerStats, float currentMana,
                      int rank) const {
  // 1. Check Cooldown
  if (currentCooldown > 0) {
    std::cout << "[Check] Ability '" << name << "' is on cooldown ("
              << currentCooldown << "s left)." << std::endl;
    return false;
  }

  // 2. Check Resource Cost
  float cost = getResourceCost(rank);
  if (currentMana < cost) {
    std::cout << "[Check] Not enough mana for '" << name << "' (" << currentMana
              << "/" << cost << ")." << std::endl;
    return false;
  }

  return true;
}

void Ability::putOnCooldown(const FinalStats &ownerStats, int rank) {
  if (rank <= 0 || rank > baseCooldowns.size()) {
    currentCooldown = 0.0f;
    return;
  }

  float baseCD = baseCooldowns[rank - 1];
  float haste = ownerStats.abilityHaste;

  // LoL Ability Haste Formula: Cooldown = Base / (1 + (Haste / 100))
  currentCooldown = baseCD / (1.0f + (haste / 100.0f));

  std::cout << "[Cooldown] '" << name << "' set to " << currentCooldown
            << "s (Base: " << baseCD << ", Haste: " << haste << ")."
            << std::endl;
}

void Ability::reduceCooldown(float amount) {
  currentCooldown -= amount;
  if (currentCooldown < 0)
    currentCooldown = 0;
}

void Ability::tick(float deltaTime) {
  if (currentCooldown > 0) {
    currentCooldown -= deltaTime;
    if (currentCooldown < 0)
      currentCooldown = 0;
  }
}

void Ability::resetCooldown() { currentCooldown = 0; }

float Ability::getResourceCost(int rank) const {
  if (rank <= 0 || rank > baseCosts.size())
    return 0.0f;
  return baseCosts[rank - 1];
}

float Ability::getCurrentCooldown() const { return currentCooldown; }

std::string Ability::getName() const { return name; }
DamageType Ability::getDamageType() const { return damageType; }
std::shared_ptr<Effect> Ability::getEffect() const { return effect; }
std::string Ability::getStatusToApply() const { return statusToApply; }
float Ability::getStatusDuration() const { return statusDuration; }

void Ability::activate() {
  if (auto timed = std::dynamic_pointer_cast<TimedEffect>(effect)) {
    timed->activate();
  } else if (effect) {
    std::cout << "[Ability] '" << name << "' activated." << std::endl;
  }
}
