#pragma once
#include "Effect.h"

class JhinPassiveEffect : public Effect {
public:
  // Reduces Crit Damage stat (175% -> 150%)
  void applyPassiveStats(Champion &owner) override;

  // Converts Attack Speed & Crit Chance into Bonus Attack Damage
  void applyScalingStats(Champion &owner) override;
};
