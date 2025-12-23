#pragma once
#include "Effect.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

// Function type for creating an effect
using EffectCreator = std::function<std::shared_ptr<Effect>()>;

class EffectRegistry {
public:
  static EffectRegistry &getInstance();

  void registerEffect(const std::string &name, EffectCreator creator);
  std::shared_ptr<Effect> createEffect(const std::string &name);

private:
  EffectRegistry() = default;
  std::map<std::string, EffectCreator> registry;
};

// Helper class for automatic registration
template <typename T> class EffectRegister {
public:
  EffectRegister(const std::string &name) {
    EffectRegistry::getInstance().registerEffect(
        name,
        []() -> std::shared_ptr<Effect> { return std::make_shared<T>(); });
  }
};
