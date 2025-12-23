#include "effects/champion/ryze/RyzePEffect.h"
#include "Champion.h"
#include "EffectRegistry.h"
#include <iostream>

static EffectRegister<RyzePEffect> ryze_p_registrar("RyzePEffect");

void RyzePEffect::onCast(Champion &owner, AbilitySlot slot) {
  if (slot == AbilitySlot::W || slot == AbilitySlot::E) {
    owner.resetAbilityCooldown(AbilitySlot::Q);
    std::cout << "[Effect] Ryze Passive resets Overload (Q) cooldown!"
              << std::endl;
  }
}

void RyzePEffect::applyScalingStats(Champion &owner) {
  const FinalStats &currentStats = owner.getTotalStats();

  float totalAP = currentStats.abilityPower;

  if (totalAP > 0) {
    float manaMultiplier = (totalAP / 100.0f) * 0.10f;

    float bonusMana = currentStats.mana * manaMultiplier;

    owner.addBonusStat(Stat::Mana, bonusMana);
  }
}
