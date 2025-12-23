#pragma once
#include "Effect.h"

class RyzePEffect : public Effect {
public:
  void applyScalingStats(Champion &owner) override;
  void onCast(Champion &owner, AbilitySlot slot) override;
};
