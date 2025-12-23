#include "strategies/jhin/JhinAutoAttackStrategy.h"
#include "Champion.h"
#include "EventBus.h"
#include <chrono>
#include <iostream>
#include <random>

float JhinAutoAttackStrategy::execute(DamageContext &ctx) {
  Champion *owner = ctx.source;
  Champion *target = ctx.target;

  // 0. Check for Reload Status
  if (owner->hasStatus("Reloading")) {
    std::cout << "[Jhin] Cannot attack while reloading!" << std::endl;
    return 0.0f;
  }

  std::cout << "[Jhin] Firing Shot " << (5 - ammo) << "/4" << std::endl;

  // 1. Initialize Base Damage
  ctx.rawPhysical = owner->getTotalStats().attackDamage;

  // 2. Gather On-Hit Effects
  for (const auto &effect : owner->getEffects()) {
    if (auto damageInstance = effect->getOnHitDamage(*owner, *target)) {
      ctx.addDamage(damageInstance->value, damageInstance->type);
    }
  }

  // 3. Pre-Calculation Hook
  EventBus::getInstance().dispatch(EventType::OnPreDamageCalc, ctx);

  // 4. Critical Strike & 4th Shot Logic
  bool forcedCrit = (ammo == 1);

  if (forcedCrit) {
    std::cout << "[Jhin] 4th Shot! Guaranteed Critical Strike + Execute."
              << std::endl;
    ctx.isCriticalStrike = true;

    // Execute Damage: 15% missing health
    float missingHP =
        target->getTotalStats().health - target->getCurrentHealth();
    float executeDamage = missingHP * 0.15f;
    ctx.rawPhysical += executeDamage;
  } else {
    // Standard Crit Check for shots 1-3
    static std::mt19937 rng(
        std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(rng) < owner->getTotalStats().criticalStrikeChance) {
      ctx.isCriticalStrike = true;
      std::cout << "[Event] Critical Strike!" << std::endl;
    }
  }

  // Apply Crit Multiplier
  if (ctx.isCriticalStrike) {
    ctx.rawPhysical *= owner->getTotalStats().criticalStrikeDamage;
  }

  // 5. Mitigation Pipeline
  const FinalStats &ownerStats = owner->getTotalStats();
  float targetArmor = target->getTotalStats().armor;

  // Percent Pen
  float effectiveArmor = targetArmor * (1.0f - ownerStats.armorPenetration);
  // Lethality
  float flatPen =
      ownerStats.lethality * (0.6f + 0.4f * owner->getLevel() / 18.0f);
  effectiveArmor -= flatPen;
  if (effectiveArmor < 0)
    effectiveArmor = 0;

  float physicalMultiplier = 100.0f / (100.0f + effectiveArmor);
  float finalPhysical = ctx.rawPhysical * physicalMultiplier;

  float targetMR = target->getTotalStats().magicResist;
  float magicMultiplier = 100.0f / (100.0f + targetMR);
  float finalMagic = ctx.rawMagic * magicMultiplier;

  ctx.finalPostMitigationDamage = finalPhysical + finalMagic + ctx.rawTrue;

  // 6. Post-Mitigation Hook
  EventBus::getInstance().dispatch(EventType::OnPostMitigation, ctx);

  // 7. Apply Damage
  target->takeDamage(ctx.finalPostMitigationDamage);

  // 8. Ammo Management (MOVED TO END)
  ammo--;
  if (ammo == 0) {
    std::cout << "[Jhin] Empty! Reloading (2.5s)..." << std::endl;
    ammo = 4;
    // Apply the reload status which blocks future attacks in step 0
    owner->applyStatus("Reloading", 2.5f);
  }

  return ctx.finalPostMitigationDamage;
}
