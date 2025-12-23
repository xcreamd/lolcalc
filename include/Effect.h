#pragma once
#include "Damage.h"
#include "Enums.h"
#include <memory>
#include <optional>
#include <string>

class Champion;

class Effect {
public:
  virtual ~Effect() = default;

  virtual void onAutoAttack(Champion &owner, Champion &target) {}

  virtual void onSpellHit(Champion &owner, Champion &target) {}

  virtual void onCast(Champion &owner, AbilitySlot slot) {}

  virtual std::optional<DamageInstance> getOnHitDamage(Champion &owner,
                                                       Champion &target) {
    return std::nullopt;
  }

  virtual float onDamageTaken(float incomingDamage) { return incomingDamage; }

  virtual void onAbilityUse(Champion &owner) {}

  virtual void applyPassiveStats(Champion &owner) {}

  virtual void applyScalingStats(Champion &owner) {}

  virtual void tick(Champion &target, float deltaTime) {}

  virtual bool isActive() const { return true; }

  virtual std::shared_ptr<Effect> clone() const { return nullptr; }
};

class TimedEffect : public Effect {
protected:
  bool active = false;
  float duration = 0.0f;
  float maxDuration = 0.0f;

public:
  TimedEffect(float duration) : maxDuration(duration), duration(duration) {}
  void activate();
  void tick(Champion &target, float deltaTime) override;
  bool isActive() const override { return active; }
};

// Represents a negative effect applied to a target (Burn, Poison)
class Debuff : public Effect {
protected:
  std::string name;
  float duration;
  float currentDuration;

public:
  Debuff(std::string name, float duration)
      : name(name), duration(duration), currentDuration(duration) {}

  void tick(Champion &target, float deltaTime) override {
    currentDuration -= deltaTime;
  }

  bool isActive() const override { return currentDuration > 0; }
  std::string getName() const { return name; }

  // Refresh duration if applied again
  void refresh() { currentDuration = duration; }
};

class DamageOverTimeEffect : public Debuff {
protected:
  float damagePerTick;
  float tickInterval;
  float timeSinceLastTick = 0.0f;
  DamageType damageType;
  Champion *source = nullptr; // Who applied this DoT?
public:
  DamageOverTimeEffect(std::string name, float duration, float damage,
                       float interval, DamageType type, Champion *source)
      : Debuff(name, duration), damagePerTick(damage), tickInterval(interval),
        damageType(type), source(source) {}

  void tick(Champion &target, float deltaTime) override;

  std::shared_ptr<Effect> clone() const override {
    return std::make_shared<DamageOverTimeEffect>(
        name, duration, damagePerTick, tickInterval, damageType, source);
  }
};
