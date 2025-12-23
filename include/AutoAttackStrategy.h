#pragma once
#include "DamageContext.h"

class Champion; // Forward declaration

class AutoAttackStrategy {
public:
  virtual ~AutoAttackStrategy() = default;
  virtual float execute(DamageContext &ctx) = 0;
};
