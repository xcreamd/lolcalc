#pragma once
#include "AutoAttackStrategy.h"

class DefaultAutoAttackStrategy : public AutoAttackStrategy {
public:
  // Ensure this matches the base class exactly
  float execute(DamageContext &ctx) override;
};
