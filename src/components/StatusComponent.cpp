#include "StatusComponent.h"
#include "Champion.h"
#include <iostream>

void StatusComponent::addEffect(std::shared_ptr<Effect> effect) {
  effects.push_back(effect);
}

void StatusComponent::applyDebuff(std::shared_ptr<Effect> debuff,
                                  const std::string &targetName) {
  if (auto d = std::dynamic_pointer_cast<Debuff>(debuff)) {
    bool found = false;
    for (auto &active : activeDebuffs) {
      if (auto activeDebuff = std::dynamic_pointer_cast<Debuff>(active)) {
        if (activeDebuff->getName() == d->getName()) {
          activeDebuff->refresh();
          std::cout << "[Debuff] Refreshed " << d->getName() << " on "
                    << targetName << std::endl;
          found = true;
          break;
        }
      }
    }
    if (!found) {
      activeDebuffs.push_back(debuff);
      std::cout << "[Debuff] Applied " << d->getName() << " to " << targetName
                << std::endl;
    }
  } else {
    activeDebuffs.push_back(debuff);
  }
}

void StatusComponent::applyStatus(const std::string &status, float duration,
                                  const std::string &targetName) {
  statusEffects[status] = duration;
  std::cout << "[Status] " << targetName << " is now " << status << " for "
            << duration << "s." << std::endl;
}

bool StatusComponent::hasStatus(const std::string &status) const {
  return statusEffects.count(status) > 0;
}

void StatusComponent::tick(Champion &owner, float deltaTime) {

  for (const auto &effect : effects) {
    effect->tick(owner, deltaTime);
  }

  for (auto it = activeDebuffs.begin(); it != activeDebuffs.end();) {
    (*it)->tick(owner, deltaTime);
    if (!(*it)->isActive()) {
      it = activeDebuffs.erase(it);
    } else {
      ++it;
    }
  }

  for (auto it = statusEffects.begin(); it != statusEffects.end();) {
    it->second -= deltaTime;
    if (it->second <= 0) {
      std::cout << "[Status] " << owner.getName() << " is no longer "
                << it->first << "." << std::endl;
      it = statusEffects.erase(it);
    } else {
      ++it;
    }
  }
}

const std::vector<std::shared_ptr<Effect>> &
StatusComponent::getEffects() const {
  return effects;
}
const std::vector<std::shared_ptr<Effect>> &
StatusComponent::getActiveDebuffs() const {
  return activeDebuffs;
}
