#pragma once
#include "AutoAttackStrategy.h"

class AsheAutoAttackStrategy : public AutoAttackStrategy {
public:
  float execute(DamageContext &ctx) override;
};
