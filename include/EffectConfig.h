#pragma once
#include "Enums.h"
#include "json.hpp"
#include <map>
#include <string>
#include <vector>

enum class TriggerType {
  OnHit,
  OnSpellHit,
  OnAutoAttack,
  OnDamageTaken,
  OnPostMitigationDamage,
  OnCrit,
  OnCast,
  Passive
};

enum class ActionType {
  DealDamage,
  ApplyDebuff,
  ApplyBuff,
  ModifyStat,
  Execute,
  DamageAmp,
  SwapForm
};

struct ScalingData {
  float base = 0.0f;
  float adRatio = 0.0f;
  float apRatio = 0.0f;
  float targetMaxHealthRatio = 0.0f;
  float targetCurrentHealthRatio = 0.0f;

  float healthDiffRatio = 0.0f;
  float healthDiffCap = 0.0f;
  float maxDamageAmp = 0.0f;
};

struct ActionConfig {
  ActionType type;
  ScalingData scaling;
  std::string effectName;
  std::string formName;
  float duration = 0.0f;
  DamageType damageType = DamageType::Physical;
  float thresholdPercent = 0.0f;
};

struct EffectRule {
  TriggerType trigger;
  std::vector<ActionConfig> actions;
};
