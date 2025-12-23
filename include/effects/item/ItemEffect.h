#pragma once
#include "Effect.h"

class BloodthirsterEffect : public Effect {
public:
  void applyPassiveStats(Champion &owner) override;
};

class InfinityEdgeEffect : public Effect {
public:
  void applyPassiveStats(Champion &owner) override;
};

class PlatedSteelcapsEffect : public Effect {
public:
  float onDamageTaken(float incomingDamage) override;
};

class LiandrysTormentEffect : public Effect {
public:
  void onSpellHit(Champion &owner, Champion &target) override;
};
