#include "Effect.h"
#include "Champion.h"
#include <iostream>

void TimedEffect::activate() {
  if (!active) {
    active = true;
    duration = maxDuration;
    std::cout << "[Effect] A timed effect has been activated!" << std::endl;
  }
}

void TimedEffect::tick(Champion &target, float deltaTime) {
  if (active) {
    duration -= deltaTime;
    if (duration <= 0) {
      active = false;
      duration = 0;
      std::cout << "[Effect] A timed effect has expired." << std::endl;
    }
  }
}

void DamageOverTimeEffect::tick(Champion &target, float deltaTime) {
  // 1. Update the time accumulator
  timeSinceLastTick += deltaTime;

  // 2. Process ticks
  // EPSILON to handle floating point imprecision causing missed ticks at exact
  // intervals
  const float EPSILON = 0.0001f;

  while (timeSinceLastTick >= tickInterval - EPSILON) {
    timeSinceLastTick -= tickInterval;

    float finalDamage = damagePerTick;
    float mitigationMultiplier = 1.0f;

    // Apply Mitigation (Matches logic in DefaultAutoAttackStrategy)
    if (damageType == DamageType::Physical) {
      float armor = target.getTotalStats().armor;

      if (source) {
        const FinalStats &sStats = source->getTotalStats();
        // 1. Percent Armor Pen
        armor *= (1.0f - sStats.armorPenetration);

        // 2. Lethality (Flat Pen)
        // Formula: Flat Pen = Lethality * (0.6 + 0.4 * level / 18)
        float flatPen =
            sStats.lethality * (0.6f + 0.4f * source->getLevel() / 18.0f);
        armor -= flatPen;
      }

      // Armor floor for penetration (reduction can go negative, pen cannot)
      if (armor < 0)
        armor = 0;

      mitigationMultiplier = 100.0f / (100.0f + armor);

    } else if (damageType == DamageType::Magic) {
      float mr = target.getTotalStats().magicResist;

      if (source) {
        const FinalStats &sStats = source->getTotalStats();
        // 1. Percent Magic Pen
        mr *= (1.0f - sStats.magicPenetrationPercent);

        // 2. Flat Magic Pen
        mr -= sStats.magicPenetrationFlat;
      }

      if (mr < 0)
        mr = 0;
      mitigationMultiplier = 100.0f / (100.0f + mr);
    }
    // True damage skips mitigation (multiplier stays 1.0)

    finalDamage *= mitigationMultiplier;

    std::cout << "[DoT] " << name << " deals " << finalDamage << " damage to "
              << target.getName() << "." << std::endl;
    target.takeDamage(finalDamage);
  }

  // 3. Tick Duration (Handled by the Debuff base class)
  Debuff::tick(target, deltaTime);
}
