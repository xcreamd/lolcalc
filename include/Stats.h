#pragma once

struct StatValue {
  float base = 0.0f;
  float growth = 0.0f;
};

struct BaseStats {
  StatValue health;
  StatValue mana;
  StatValue attackDamage;
  StatValue attackSpeedRatio;
  StatValue armor;
  StatValue magicResist;
  StatValue movementSpeed;
};

struct FinalStats {
  float health = 0.0f;
  float mana = 0.0f;
  float attackDamage = 0.0f;
  float abilityPower = 0.0f;
  float armor = 0.0f;
  float magicResist = 0.0f;
  float attackSpeed = 0.0f;
  float abilityHaste = 0.0f;
  float criticalStrikeChance = 0.0f;
  float criticalStrikeDamage = 1.75f;

  float armorPenetration = 0.0f;
  float lethality = 0.0f;
  float magicPenetrationPercent = 0.0f;
  float magicPenetrationFlat = 0.0f;

  float movementSpeed = 0.0f;
};
