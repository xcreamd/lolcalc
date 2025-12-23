#include "strategies/DefaultAutoAttackStrategy.h"
#include "Champion.h"
#include "EventBus.h"
#include <chrono>
#include <iostream>
#include <random>

float DefaultAutoAttackStrategy::execute(DamageContext &ctx) {
  Champion *owner = ctx.source;
  Champion *target = ctx.target;

  // --- 1. Initialize Base Damage ---
  // Start with the champion's total AD.
  // We store this in the context so listeners can see/modify it.
  ctx.rawPhysical = owner->getTotalStats().attackDamage;

  // --- 2. Gather On-Hit Effects ---
  // Iterate through items/passives and accumulate their on-hit damage into the
  // context.
  for (const auto &effect : owner->getEffects()) {
    if (auto damageInstance = effect->getOnHitDamage(*owner, *target)) {
      ctx.addDamage(damageInstance->value, damageInstance->type);
    }
  }

  // --- 3. Pre-Calculation Hook ---
  // Allow systems (like Draven's Q) to modify raw values before
  // crit/mitigation.
  EventBus::getInstance().dispatch(EventType::OnPreDamageCalc, ctx);

  // --- 4. Critical Strike Calculation ---
  // We check for crit *before* adding the on-hit bonus if the game logic
  // requires it, but typically Crit multiplies Base AD, not On-Hit effects.
  // However, Draven Q is a specific edge case where the bonus IS multiplied.
  // For standard Auto Attacks, we multiply the current rawPhysical (which is
  // Total AD).

  static std::mt19937 rng(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  if (dist(rng) < owner->getTotalStats().criticalStrikeChance) {
    std::cout << "[Event] Critical Strike!" << std::endl;
    ctx.isCriticalStrike = true;

    // Apply Crit Multiplier to the physical component
    // Note: This assumes rawPhysical currently only contains things that CAN
    // crit. If we had distinct "On-Hit Physical" vs "Base Physical", we would
    // separate them. For this simplified engine, we scale the total physical
    // accumulated so far.
    ctx.rawPhysical *= owner->getTotalStats().criticalStrikeDamage;
  }

  // --- 5. Mitigation Pipeline ---
  const FinalStats &ownerStats = owner->getTotalStats();
  float targetArmor = target->getTotalStats().armor;

  // A. Percent Armor Penetration
  float effectiveArmor = targetArmor * (1.0f - ownerStats.armorPenetration);

  // B. Lethality (Flat Pen)
  // Formula: Flat Pen = Lethality * (0.6 + 0.4 * level / 18)
  float flatPen =
      ownerStats.lethality * (0.6f + 0.4f * owner->getLevel() / 18.0f);
  effectiveArmor -= flatPen;

  // Armor floor
  if (effectiveArmor < 0)
    effectiveArmor = 0;

  // Physical Calculation
  float physicalMultiplier = 100.0f / (100.0f + effectiveArmor);
  float finalPhysical = ctx.rawPhysical * physicalMultiplier;

  // Magic Calculation (Simplified)
  float targetMR = target->getTotalStats().magicResist;
  float effectiveMR =
      (targetMR > 0) ? targetMR : 0; // Missing Pen logic for brevity
  float magicMultiplier = 100.0f / (100.0f + effectiveMR);
  float finalMagic = ctx.rawMagic * magicMultiplier;

  // Sum it up
  ctx.finalPostMitigationDamage = finalPhysical + finalMagic + ctx.rawTrue;

  // --- 6. Post-Mitigation Hook ---
  // Useful for defensive items like Plated Steelcaps
  EventBus::getInstance().dispatch(EventType::OnPostMitigation, ctx);

  // --- 7. Apply Damage ---
  target->takeDamage(ctx.finalPostMitigationDamage);

  // owner->heal(finalPhysical * ownerStats.lifeSteal);
  // owner->heal((finalPhysical + finalMagic) * ownerStats.omnivamp);
  float totalLifeSteal = owner->getTotalStats().lifeSteal;
  float totalOmnivamp = owner->getTotalStats().omnivamp;
  float healAmount = 0.0f;

  if (totalLifeSteal > 0) {
    // Life Steal applies to physical damage of AA (Base + Physical On-Hits)
    healAmount += finalPhysical * totalLifeSteal;
  }

  if (totalOmnivamp > 0) {
    healAmount += ctx.finalPostMitigationDamage * totalOmnivamp;
  }

  if (healAmount > 0) {
    owner->heal(healAmount);
    std::cout << "[Sustain] " << owner->getName() << " healed for "
              << healAmount << " HP." << std::endl;
  }
  return ctx.finalPostMitigationDamage;
}
