#pragma once
#include "Effect.h"
#include "Enums.h"
#include <map>
#include <memory>
#include <string>

class Item {
public:
  Item(const std::string &name, const std::map<Stat, float> &stats,
       std::shared_ptr<Effect> effect = nullptr);
  std::string getName() const;
  const std::map<Stat, float> &getStats() const;
  std::shared_ptr<Effect> getEffect() const;

private:
  std::string name;
  std::map<Stat, float> stats;
  std::shared_ptr<Effect> effect;
};
