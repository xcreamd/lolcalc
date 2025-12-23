#include "effects/champion/draven/DravenWEffect.h"
#include "Champion.h"
#include "Enums.h"
#include <iostream>
#include <vector>

void DravenWEffect::applyPassiveStats(Champion &owner) {
  if (isActive()) {
    const auto &ranks = owner.getAbilityRanks();
    auto it = ranks.find(AbilitySlot::W);
    if (it == ranks.end() || it->second == 0)
      return;
    int rank = it->second;

    std::vector<float> attackSpeedBonus = {0.20f, 0.25f, 0.30f, 0.35f, 0.40f};
    std::vector<float> moveSpeedBonus = {0.40f, 0.45f, 0.50f, 0.55f, 0.60f};

    owner.addBonusStat(Stat::AttackSpeed, attackSpeedBonus.at(rank - 1));
    owner.addBonusStat(Stat::MovementSpeed, moveSpeedBonus.at(rank - 1));
    std::cout << "[Effect] Blood Rush is active!" << std::endl;
  }
}

void DravenWEffect::resetCooldown() {
  // TODO: reset cd ??
  std::cout << "[Effect] Blood Rush cooldown has been reset!" << std::endl;
}
