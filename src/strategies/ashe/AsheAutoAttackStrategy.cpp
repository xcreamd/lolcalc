#include "strategies/ashe/AsheAutoAttackStrategy.h"
#include "Champion.h"
#include "effects/champion/ashe/AsheQEffect.h" // Required for Q logic
#include <chrono>
#include <iostream>
#include <random>

float AsheAutoAttackStrategy::execute(DamageContext &ctx) {
  Champion *owner = ctx.source;
  Champion *target = ctx.target;

  // 1. Collect On-Hit Damage (Legacy)
  for (const auto &effect : owner->getEffects()) {
    if (auto damageInstance = effect->getOnHitDamage(*owner, *target)) {
      ctx.addDamage(damageInstance->value, damageInstance->type);
    }
  }

  // 2. Ashe Passive Logic
  float totalAD = owner->getTotalStats().attackDamage;
  float critChance = owner->getTotalStats().criticalStrikeChance;

  // Base damage modifier: 110% + (100% * CritChance)
  // Note: IE buff to crit damage should ideally be read from stats here too
  float damageModifier = 1.1f + (1.0f * critChance);

  // Check Ranger's Focus (Q)
  for (const auto &effect : owner->getEffects()) {
    if (auto asheQ = std::dynamic_pointer_cast<AsheQEffect>(effect)) {
      if (asheQ->isActive()) {
        // Simplified rank assumption (should fetch from abilityRanks)
        damageModifier = 1.25f;
        std::cout << "[Effect] Ranger's Focus Flurry!" << std::endl;
      }
    }
  }

  // Add Ashe's unique base damage to the physical bucket
  ctx.rawPhysical += (totalAD * damageModifier);

  // 3. Mitigation
  float targetArmor = target->getTotalStats().armor *
                      (1.0f - owner->getTotalStats().armorPenetration);
  float physMult = 100.0f / (100.0f + (targetArmor > 0 ? targetArmor : 0));

  float targetMR = target->getTotalStats().magicResist;
  float magicMult = 100.0f / (100.0f + (targetMR > 0 ? targetMR : 0));

  ctx.finalPostMitigationDamage =
      (ctx.rawPhysical * physMult) + (ctx.rawMagic * magicMult) + ctx.rawTrue;

  // 4. Apply
  target->takeDamage(ctx.finalPostMitigationDamage);
  return ctx.finalPostMitigationDamage;
}
