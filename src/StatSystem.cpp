#include "StatSystem.h"
#include <iostream>

StatSystem::StatSystem() { stats[Stat::CriticalStrikeDamage].base = 1.75f; }

void StatSystem::setBase(Stat stat, float value) {
  stats[stat].base = value;
  isDirty = true;
}

void StatSystem::setGrowth(Stat stat, float value) {
  stats[stat].growth = value;
  isDirty = true;
}

void StatSystem::setLevel(int newLevel) {
  level = newLevel;
  isDirty = true;
}

void StatSystem::addBonus(Stat stat, float value) {
  stats[stat].flatBonus += value;
  isDirty = true;
}

void StatSystem::addMultiplier(Stat stat, float percent) {
  stats[stat].multiplier += percent;
  isDirty = true;
}

void StatSystem::clearBonuses() {
  for (auto &[key, entry] : stats) {
    entry.flatBonus = 0.0f;
    entry.percentBonus = 0.0f;
    entry.multiplier = 0.0f;
  }
  isDirty = true;
}

float StatSystem::calculateStatAtLevel(float base, float growth,
                                       int lvl) const {
  if (lvl <= 1)
    return base;
  float n = static_cast<float>(lvl);
  return base + growth * (n - 1) * (0.7025f + 0.0175f * (n - 1));
}

void StatSystem::recalculate() {
  auto getVal = [&](Stat s) {
    StatEntry &e = stats[s];
    float baseVal = calculateStatAtLevel(e.base, e.growth, level);

    if (s == Stat::AttackSpeed) {
      float baseRatio = e.base;

      float growthBonus =
          e.growth * (level - 1) * (0.7025f + 0.0175f * (level - 1));

      return baseRatio * (1.0f + growthBonus + e.flatBonus);
    }

    return (baseVal + e.flatBonus) * (1.0f + e.multiplier);
  };

  cachedFinalStats.health = getVal(Stat::Health);
  cachedFinalStats.mana = getVal(Stat::Mana);
  cachedFinalStats.attackDamage = getVal(Stat::AttackDamage);
  cachedFinalStats.abilityPower = getVal(Stat::AbilityPower);
  cachedFinalStats.armor = getVal(Stat::Armor);
  cachedFinalStats.magicResist = getVal(Stat::MagicResist);
  cachedFinalStats.attackSpeed = getVal(Stat::AttackSpeed);

  cachedFinalStats.abilityHaste = getVal(Stat::AbilityHaste);

  cachedFinalStats.criticalStrikeChance = getVal(Stat::CriticalStrikeChance);
  cachedFinalStats.criticalStrikeDamage = getVal(Stat::CriticalStrikeDamage);

  cachedFinalStats.armorPenetration = getVal(Stat::ArmorPenetration);
  cachedFinalStats.lethality = getVal(Stat::Lethality);
  cachedFinalStats.magicPenetrationPercent =
      getVal(Stat::MagicPenetrationPercent);
  cachedFinalStats.magicPenetrationFlat = getVal(Stat::MagicPenetrationFlat);

  cachedFinalStats.movementSpeed = getVal(Stat::MovementSpeed);
  cachedFinalStats.lifeSteal = getVal(Stat::LifeSteal);
  cachedFinalStats.omnivamp = getVal(Stat::OmniVamp);

  isDirty = false;
}

float StatSystem::getTotal(Stat stat) {
  if (isDirty)
    recalculate();

  switch (stat) {
  case Stat::Health:
    return cachedFinalStats.health;
  case Stat::Mana:
    return cachedFinalStats.mana;
  case Stat::AttackDamage:
    return cachedFinalStats.attackDamage;
  case Stat::AbilityPower:
    return cachedFinalStats.abilityPower;
  case Stat::Armor:
    return cachedFinalStats.armor;
  case Stat::MagicResist:
    return cachedFinalStats.magicResist;
  case Stat::AttackSpeed:
    return cachedFinalStats.attackSpeed;
  case Stat::AbilityHaste:
    return cachedFinalStats.abilityHaste; // NEW
  case Stat::CriticalStrikeChance:
    return cachedFinalStats.criticalStrikeChance;
  case Stat::CriticalStrikeDamage:
    return cachedFinalStats.criticalStrikeDamage;
  case Stat::ArmorPenetration:
    return cachedFinalStats.armorPenetration;
  case Stat::Lethality:
    return cachedFinalStats.lethality;
  case Stat::MagicPenetrationPercent:
    return cachedFinalStats.magicPenetrationPercent;
  case Stat::MagicPenetrationFlat:
    return cachedFinalStats.magicPenetrationFlat;
  case Stat::MovementSpeed:
    return cachedFinalStats.movementSpeed;
  case Stat::LifeSteal:
    return cachedFinalStats.lifeSteal;
  case Stat::OmniVamp:
    return cachedFinalStats.omnivamp;
  }
  return 0.0f;
}

const FinalStats &StatSystem::getFinalStats() {
  if (isDirty)
    recalculate();
  return cachedFinalStats;
}

float StatSystem::getBase(Stat stat) const {
  if (stats.count(stat) == 0)
    return 0.0f;

  if (stat == Stat::AttackSpeed)
    return stats.at(stat).base;

  return calculateStatAtLevel(stats.at(stat).base, stats.at(stat).growth,
                              level);
}

float StatSystem::getBonus(Stat stat) const {
  if (stats.count(stat) == 0)
    return 0.0f;
  return stats.at(stat).flatBonus;
}
