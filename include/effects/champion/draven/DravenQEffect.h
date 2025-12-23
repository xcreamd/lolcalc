#pragma once
#include "Effect.h"

class DravenQEffect : public Effect {
public:
  std::optional<DamageInstance> getOnHitDamage(Champion &owner,
                                               Champion &target) override;
};
