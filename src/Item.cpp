#include "Item.h"

Item::Item(const std::string &name, const std::map<Stat, float> &stats,
           std::shared_ptr<Effect> effect)
    : name(name), stats(stats), effect(effect) {}

std::string Item::getName() const { return name; }
const std::map<Stat, float> &Item::getStats() const { return stats; }
std::shared_ptr<Effect> Item::getEffect() const { return effect; }
