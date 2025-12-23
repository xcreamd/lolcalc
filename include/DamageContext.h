#pragma once
#include "Enums.h"
#include <string>

class Champion;

struct DamageContext {
  Champion *source;
  Champion *target;

  std::string abilityName = "AutoAttack";
  bool isBasicAttack = false;

  float rawPhysical = 0.0f;
  float rawMagic = 0.0f;
  float rawTrue = 0.0f;

  bool isCriticalStrike = false;
  float finalPostMitigationDamage = 0.0f;

  // Helper to add damage
  void addDamage(float amount, DamageType type) {
    switch (type) {
    case DamageType::Physical:
      rawPhysical += amount;
      break;
    case DamageType::Magic:
      rawMagic += amount;
      break;
    case DamageType::True:
      rawTrue += amount;
      break;
    }
  }
};
