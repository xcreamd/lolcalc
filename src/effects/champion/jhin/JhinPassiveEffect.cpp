#include "effects/champion/jhin/JhinPassiveEffect.h"
#include "Champion.h"
#include "EffectRegistry.h"
#include <iostream>

static EffectRegister<JhinPassiveEffect> jhin_p_registrar("JhinPassiveEffect");

void JhinPassiveEffect::applyPassiveStats(Champion &owner) {
  // Jhin's crits deal reduced damage.
  // Base is 175%, Jhin is 150.5% (approx 150%).
  // We apply a negative bonus to the Crit Damage stat.
  owner.addBonusStat(Stat::CriticalStrikeDamage, -0.25f);
}

void JhinPassiveEffect::applyScalingStats(Champion &owner) {
  // Whisper: Attack Speed is fixed, but bonus AS is converted to AD.
  // Also gains bonus AD from Crit Chance.

  const FinalStats &stats = owner.getTotalStats();

  // 1. Calculate Bonus Attack Speed %
  // Jhin's base AS ratio is 0.625.
  float baseAS = owner.getBaseStatsData().attackSpeedRatio.base;
  float currentAS = stats.attackSpeed;

  // This is a simplification: In the real game, Jhin's AS is hard-locked.
  // Here we calculate how much "extra" AS he has from items/growth.
  float bonusASPercent = 0.0f;
  if (currentAS > baseAS) {
    bonusASPercent = (currentAS - baseAS) / baseAS;
  }

  // 2. Calculate Multiplier
  // 25% AD per 100% Bonus AS + 4% AD per 10% Crit Chance
  float conversionFromAS = bonusASPercent * 0.25f;
  float conversionFromCrit = stats.criticalStrikeChance * 0.40f;
  float totalAdMultiplier = conversionFromAS + conversionFromCrit;

  // 3. Apply Multiplier to Total AD
  // Note: We need to be careful not to create an infinite loop if we read AD
  // here. We want to multiply the AD calculated *so far*. Since
  // applyScalingStats runs after items, stats.attackDamage holds (Base +
  // Items).

  float bonusAD = stats.attackDamage * totalAdMultiplier;

  if (bonusAD > 0) {
    owner.addBonusStat(Stat::AttackDamage, bonusAD);
    // std::cout << "[Jhin Passive] Converted stats to " << bonusAD << " Bonus
    // AD." << std::endl;
  }
}
