#include "EffectRegistry.h"
#include <memory>

EffectRegistry &EffectRegistry::getInstance() {
  static EffectRegistry instance;
  return instance;
}

void EffectRegistry::registerEffect(const std::string &name,
                                    EffectCreator creator) {
  registry[name] = creator;
}

std::unique_ptr<Effect> EffectRegistry::createEffect(const std::string &name) {
  if (registry.count(name)) {
    return registry[name](); // Call the creation function
  }
  return nullptr; // Effect not found
}
