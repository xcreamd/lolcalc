#include "components/StatsComponent.h"

StatsComponent::StatsComponent(const BaseStats &baseData)
    : baseStatsData(baseData) {

  // Initialize StatSystem
  statSystem.setBase(Stat::Health, baseData.health.base);
  statSystem.setGrowth(Stat::Health, baseData.health.growth);
  statSystem.setBase(Stat::Mana, baseData.mana.base);
  statSystem.setGrowth(Stat::Mana, baseData.mana.growth);
  statSystem.setBase(Stat::AttackDamage, baseData.attackDamage.base);
  statSystem.setGrowth(Stat::AttackDamage, baseData.attackDamage.growth);
  statSystem.setBase(Stat::Armor, baseData.armor.base);
  statSystem.setGrowth(Stat::Armor, baseData.armor.growth);
  statSystem.setBase(Stat::MagicResist, baseData.magicResist.base);
  statSystem.setGrowth(Stat::MagicResist, baseData.magicResist.growth);
  statSystem.setBase(Stat::AttackSpeed, baseData.attackSpeedRatio.base);
  statSystem.setGrowth(Stat::AttackSpeed, baseData.attackSpeedRatio.growth);
  statSystem.setBase(Stat::MovementSpeed, baseData.movementSpeed.base);

  statSystem.setLevel(level);
}

void StatsComponent::setLevel(int newLevel) {
  level = newLevel;
  statSystem.setLevel(newLevel);
}

int StatsComponent::getLevel() const { return level; }

void StatsComponent::addBonusStat(Stat stat, float value) {
  statSystem.addBonus(stat, value);
}

void StatsComponent::clearBonuses() {
  statSystem.clearBonuses();
  // Default Crit Damage Base
  statSystem.addBonus(Stat::CriticalStrikeDamage,
                      1.75f - statSystem.getBase(Stat::CriticalStrikeDamage));
}

void StatsComponent::calculateTotalStats() {
  // StatSystem handles the calculation lazily when getFinalStats is called
  // but we can force it here if needed, or manage other dependencies.
  // For now, this is a pass-through to ensure the system is ready.
  statSystem.getFinalStats();
}

const FinalStats &StatsComponent::getTotalStats() {
  return statSystem.getFinalStats();
}

const BaseStats &StatsComponent::getBaseStatsData() const {
  return baseStatsData;
}

float StatsComponent::getBaseStatValue(Stat stat) const {
  return statSystem.getBase(stat);
}
