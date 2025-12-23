#pragma once
#include "Effect.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Champion;

class StatusComponent {
public:
  void addEffect(std::shared_ptr<Effect> effect);

  void applyDebuff(std::shared_ptr<Effect> debuff,
                   const std::string &targetName);

  void applyStatus(const std::string &status, float duration,
                   const std::string &targetName);
  bool hasStatus(const std::string &status) const;

  void tick(Champion &owner, float deltaTime);

  const std::vector<std::shared_ptr<Effect>> &getEffects() const;
  const std::vector<std::shared_ptr<Effect>> &getActiveDebuffs() const;

private:
  std::vector<std::shared_ptr<Effect>> effects;
  std::vector<std::shared_ptr<Effect>> activeDebuffs;
  std::map<std::string, float> statusEffects;
};
