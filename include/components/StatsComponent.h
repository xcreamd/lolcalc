#pragma once
#include "StatSystem.h"
#include "Stats.h"

class StatsComponent {
public:
  StatsComponent(const BaseStats &baseData);

  void setLevel(int level);
  int getLevel() const;

  void addBonusStat(Stat stat, float value);
  void clearBonuses();

  // Forces a recalculation via StatSystem
  void calculateTotalStats();

  const FinalStats &getTotalStats();
  const BaseStats &getBaseStatsData() const;

  // Helper to calculate base stat at current level
  float getBaseStatValue(Stat stat) const;

private:
  BaseStats baseStatsData;
  StatSystem statSystem;
  int level = 1;
};
