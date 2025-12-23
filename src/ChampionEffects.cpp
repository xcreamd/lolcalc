// TODO: fix deprecated
// #include "ChampionEffects.h"
// #include "Champion.h"
// #include <iostream>
//
// std::optional<DamageInstance> DravenQEffect::getOnHitDamage(Champion &owner,
//                                                             Champion &target)
//                                                             {
//   const auto &ranks = owner.getAbilityRanks();
//   auto it = ranks.find("Q");
//   if (it == ranks.end() || it->second == 0) {
//     return std::nullopt; // Q is not ranked, no bonus damage
//   }
//   int rank = it->second;
//
//   // formula
//   std::vector<float> baseDamage = {40, 45, 50, 55, 60};
//   std::vector<float> bonusAdScaling = {0.75f, 0.85f, 0.95f, 1.05f, 1.15f};
//
//   const auto &baseStats = owner.getBaseStats();
//   const auto &totalStats = owner.getTotalStats();
//   float bonusAD = totalStats.attackDamage -
//                   (baseStats.attackDamage.base +
//                    baseStats.attackDamage.growth * (owner.getLevel() - 1));
//
//   float totalBonusDamage =
//       baseDamage.at(rank - 1) + (bonusAD * bonusAdScaling.at(rank - 1));
//
//   std::cout << "[Effect] Spinning Axe bonus -> " << totalBonusDamage
//             << std::endl;
//   return DamageInstance{totalBonusDamage, DamageType::Physical};
// }
//
// // TODO: fix BT current patch
// void BloodthirsterEffect::applyPassiveStats(Champion &owner) {
//   // if (owner.getCurrentHealth() / owner.getTotalStats().health > 0.5) {
//   //   owner.addBonusStat(Stat::AttackDamage, 35);
//   // }
// }
//
// void InfinityEdgeEffect::applyPassiveStats(Champion &owner) {
//   owner.addBonusStat(Stat::CriticalStrikeDamage, 0.40f);
// }
