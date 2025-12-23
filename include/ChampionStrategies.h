#pragma once
#include "AutoAttackStrategy.h"

class DefaultAutoAttackStrategy : public AutoAttackStrategy {
public:
  float execute(DamageContext &ctx) override;
};

// Note: Ashe's strategy will need updating in the src file too
class AsheAutoAttackStrategy : public AutoAttackStrategy {
public:
  float execute(DamageContext &ctx) override;
};
