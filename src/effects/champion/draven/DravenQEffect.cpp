#include "effects/champion/draven/DravenQEffect.h"
#include "Champion.h"
#include "effects/champion/draven/DravenWEffect.h"
#include <iostream>
#include <vector>

std::optional<DamageInstance> DravenQEffect::getOnHitDamage(Champion &owner,
                                                            Champion &target) {
  const auto &ranks = owner.getAbilityRanks();
  auto it = ranks.find(AbilitySlot::Q);
  if (it == ranks.end() || it->second == 0) {
    return std::nullopt;
  }
  int rank = it->second;

  std::vector<float> baseDamage = {40, 45, 50, 55, 60};
  std::vector<float> bonusAdScaling = {0.75f, 0.85f, 0.95f, 1.05f, 1.15f};

  const auto &baseStats = owner.getBaseStatsData();
  const auto &totalStats = owner.getTotalStats();

  // Recalculate base AD at current level to determine Bonus AD
  // Formula: Base + Growth * (n - 1) * (0.7025 + 0.0175 * (n - 1))
  float currentBaseAD = baseStats.attackDamage.base;
  int level = owner.getLevel();
  if (level > 1) {
    float n = static_cast<float>(level);
    currentBaseAD +=
        baseStats.attackDamage.growth * (n - 1) * (0.7025f + 0.0175f * (n - 1));
  }

  float bonusAD = totalStats.attackDamage - currentBaseAD;

  float totalBonusDamage =
      baseDamage.at(rank - 1) + (bonusAD * bonusAdScaling.at(rank - 1));

  // Logic to reset W cooldown if active
  for (const auto &effect : owner.getEffects()) {
    if (auto wEffect = std::dynamic_pointer_cast<DravenWEffect>(effect)) {
      wEffect->resetCooldown();
      break;
    }
  }

  std::cout << "[Effect] Spinning Axe bonus! -> ";
  return DamageInstance{totalBonusDamage, DamageType::Physical};
}
