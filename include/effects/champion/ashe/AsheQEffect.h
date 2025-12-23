#pragma once
#include "Effect.h"

class AsheQEffect : public TimedEffect {
private:
  int focusStacks = 0;

public:
  AsheQEffect() : TimedEffect(4.0f) {}
  void onAutoAttack(Champion &owner, Champion &target) override;
  void activate();
  void applyPassiveStats(Champion &owner) override;
};
