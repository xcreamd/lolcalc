// G
// #include "Champion.h"
// #include <chrono>
// #include <iostream>
// #include <random>
//
// float DefaultAutoAttackStrategy::execute(Champion &owner, Champion &target) {
//   float totalBonusPhysicalOnHit = 0;
//   float totalBonusMagicOnHit = 0;
//   float totalBonusTrueOnHit = 0;
//
//   // for (const auto &effect : owner.getEffects()) {
//   //   if (auto damageInstance = effect->getOnHitDamage(owner, target)) {
//   //     switch (damageInstance->type) {
//   //     case DamageType::Physical:
//   //       totalBonusPhysicalOnHit += damageInstance->value;
//   //       break;
//   //     case DamageType::Magic:
//   //       totalBonusMagicOnHit += damageInstance->value;
//   //       break;
//   //     case DamageType::True:
//   //       totalBonusTrueOnHit += damageInstance->value;
//   //       break;
//   //     }
//   //   }
//   // }
//
//   float basePhysicalDamage =
//       owner.getTotalStats().attackDamage + totalBonusPhysicalOnHit;
//
//   static std::mt19937 rng(
//       std::chrono::steady_clock::now().time_since_epoch().count());
//   std::uniform_real_distribution<float> dist(0.0f, 1.0f);
//
//   if (dist(rng) < owner.getTotalStats().criticalStrikeChance) {
//     std::cout << "[Event] Critical Strike!" << std::endl;
//     basePhysicalDamage *= owner.getTotalStats().criticalStrikeDamage;
//   }
//
//   float totalPreMitigationPhysical = basePhysicalDamage;
//   float targetArmor = target.getTotalStats().armor *
//                       (1.0f - owner.getBonusStats().armorPenetration);
//   float physicalMultiplier =
//       100.0f / (100.0f + (targetArmor > 0 ? targetArmor : 0));
//   float finalPhysicalDamage = totalPreMitigationPhysical *
//   physicalMultiplier;
//
//   float targetMR = target.getTotalStats().magicResist;
//   float magicMultiplier = 100.0f / (100.0f + (targetMR > 0 ? targetMR : 0));
//   float finalMagicDamage = totalBonusMagicOnHit * magicMultiplier;
//
//   float totalDamage =
//       finalPhysicalDamage + finalMagicDamage + totalBonusTrueOnHit;
//   target.takeDamage(totalDamage);
//   return totalDamage;
// }
