#pragma once

enum class Stat {
  Health,
  Mana,
  AttackDamage,
  AbilityPower,
  Armor,
  MagicResist,
  AttackSpeed,
  CriticalStrikeChance,
  CriticalStrikeDamage,
  ArmorPenetration,
  Lethality,
  MagicPenetrationPercent,
  MagicPenetrationFlat,
  MovementSpeed,
  AbilityHaste
};

enum class DamageType { Physical, Magic, True };

enum class EventType {
  OnPreDamageCalc,
  OnCritRoll,
  OnHit,
  OnPostMitigation,
  OnSpellHit
};

enum class AbilitySlot {
  Passive,
  Q,
  W,
  E,
  R,
  Item1,
  Item2,
  Item3,
  Item4,
  Item5,
  Item6,
  Trinket,
  Unknown
};
