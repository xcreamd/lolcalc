#pragma once
#include "AutoAttackStrategy.h"

class JhinAutoAttackStrategy : public AutoAttackStrategy {
private:
  int ammo = 4;

public:
  float execute(DamageContext &ctx) override;
};
