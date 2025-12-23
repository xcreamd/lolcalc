#pragma once
#include "Effect.h"

class DravenWEffect : public TimedEffect {
public:
  DravenWEffect() : TimedEffect(1.5f) {}
  void applyPassiveStats(Champion &owner) override;
  void resetCooldown();
};
