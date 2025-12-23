#pragma once
#include "Enums.h"
#include "Stats.h"
#include <map>

class StatSystem {
public:
  StatSystem();

  void setBase(Stat stat, float value);
  void setGrowth(Stat stat, float value);
  void setLevel(int level);

  void addBonus(Stat stat, float value);
  void addMultiplier(Stat stat, float percent);

  void clearBonuses();

  float getTotal(Stat stat);
  float getBase(Stat stat) const;
  float getBonus(Stat stat) const;

  const FinalStats &getFinalStats();

private:
  struct StatEntry {
    float base = 0.0f;
    float growth = 0.0f;
    float flatBonus = 0.0f;
    float percentBonus = 0.0f;
    float multiplier = 0.0f;
  };

  std::map<Stat, StatEntry> stats;
  int level = 1;
  bool isDirty = true;
  FinalStats cachedFinalStats;

  void recalculate();
  float calculateStatAtLevel(float base, float growth, int lvl) const;
};
