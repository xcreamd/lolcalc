
#include "effects/champion/ashe/AsheQEffect.h"
#include "Champion.h"
#include "Enums.h"
#include <iostream>
#include <vector>

void AsheQEffect::onAutoAttack(Champion &owner, Champion &target) {
  if (isActive())
    return;

  if (focusStacks < 4) {
    focusStacks++;
    std::cout << "[Effect] Ranger's Focus stacks to " << focusStacks
              << std::endl;
  }
}

void AsheQEffect::activate() {
  if (focusStacks == 4) {
    std::cout << "[Ability] Ashe activates Ranger's Focus!" << std::endl;
    TimedEffect::activate();
    focusStacks = 0;
  } else {
    std::cout << "[Ability] Ashe tries to use Q, but not enough Focus!"
              << std::endl;
  }
}

void AsheQEffect::applyPassiveStats(Champion &owner) {
  if (isActive()) {
    const auto &ranks = owner.getAbilityRanks();
    auto it = ranks.find(AbilitySlot::Q);
    if (it == ranks.end() || it->second == 0)
      return;
    int rank = it->second;

    std::vector<float> attackSpeedBonus = {0.20f, 0.25f, 0.30f, 0.35f, 0.40f};
    owner.addBonusStat(Stat::AttackSpeed, attackSpeedBonus.at(rank - 1));
  }
}
