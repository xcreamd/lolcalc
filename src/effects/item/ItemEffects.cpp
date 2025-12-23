#include "Champion.h"
#include "EffectRegistry.h"
#include "effects/item/ItemEffect.h"
#include <algorithm> // for std::min
#include <iostream>

// --- EFFECT REGISTRATION ---
static EffectRegister<BloodthirsterEffect>
    bloodthirster_registrar("BloodthirsterEffect");
static EffectRegister<InfinityEdgeEffect>
    infinity_edge_registrar("InfinityEdgeEffect");
static EffectRegister<PlatedSteelcapsEffect>
    plated_steelcaps_registrar("PlatedSteelcapsEffect");
static EffectRegister<LiandrysTormentEffect>
    liandrys_registrar("LiandrysTormentEffect");
// --- END REGISTRATION ---

void BloodthirsterEffect::applyPassiveStats(Champion &owner) {
  if (owner.getCurrentHealth() / owner.getTotalStats().health > 0.5) {
    owner.addBonusStat(Stat::AttackDamage, 35);
  }
}

void InfinityEdgeEffect::applyPassiveStats(Champion &owner) {
  owner.addBonusStat(Stat::CriticalStrikeDamage, 0.40f);
}

float PlatedSteelcapsEffect::onDamageTaken(float incomingDamage) {
  std::cout << "[Effect] Plated Steelcaps reduces incoming damage!"
            << std::endl;
  return incomingDamage * 0.88f;
}

// --- Liandry's Torment Implementation ---

// Helper Class: The "Suffering" Buff (Applied to Owner)
// Tracks time in combat and calculates the damage increase (0% - 6%)
class LiandrysSufferingBuff : public TimedEffect {
private:
  float combatTimer = 0.0f;

public:
  LiandrysSufferingBuff()
      : TimedEffect(5.0f) {} // Lasts 5s after leaving combat

  void tick(Champion &owner, float deltaTime) override {
    // Run standard timer logic (expiry)
    TimedEffect::tick(owner, deltaTime);

    if (isActive()) {
      combatTimer += deltaTime;

      // Calculate stacks: 1 stack per second, max 3
      int stacks = std::min(3, static_cast<int>(combatTimer));
      float damageAmp = stacks * 0.02f; // 2% per stack

      // In a full engine, we would apply this to a 'DamageMultiplier' stat.
      // For now, we log it to prove the simulation works.
      if (stacks > 0) {
        // std::cout << "[Liandry] Suffering Stacks: " << stacks << " (+" <<
        // (damageAmp * 100) << "% Dmg)" << std::endl;
      }
    }
  }

  // Call this when combat continues to prevent expiry and keep stacking
  void refresh() {
    active = true;
    duration = maxDuration;
    // Note: We DO NOT reset combatTimer here, because we want it to keep
    // stacking up to 3s
  }

  float getDamageAmp() const {
    int stacks = std::min(3, static_cast<int>(combatTimer));
    return 1.0f + (stacks * 0.02f);
  }

  // Required for dynamic_cast checks
  std::string getName() const { return "LiandrysSufferingBuff"; }
};

void LiandrysTormentEffect::onSpellHit(Champion &owner, Champion &target) {
  // 1. Apply "Suffering" Buff to Owner (Ramps up damage)
  // We check if the owner already has the buff to update it
  bool foundBuff = false;
  float currentAmp = 1.0f;

  // This part requires access to owner's active effects, which isn't directly
  // exposed nicely yet. In a robust system, we'd use
  // owner.getEffect<LiandrysSufferingBuff>(). For this simulation step, we'll
  // just instantiate/refresh it blindly or assume 0% for the calculation below
  // to keep the "burn" logic clean, OR we can try to find it.

  // (Simplified) Calculate the Burn Damage
  // Passive: Burn for 2% max HP per second
  float damagePerSec = target.getTotalStats().health * 0.02f;

  // Apply Suffering Amp to the Burn itself?
  // Usually item passives benefit from the item's own damage amp.
  // Let's assume we are at 0 stacks for the initial hit calculation for
  // simplicity, or we'd need to store the Buff on the Champion first.

  auto burn = std::make_shared<DamageOverTimeEffect>(
      "Liandry's Burn", 3.0f, damagePerSec, 1.0f, DamageType::Magic, &owner);

  target.applyDebuff(burn);

  // 2. Manage the Suffering Buff on the Owner
  // Since we don't have a clean "getBuff" API in Champion yet, we'll create a
  // new one In a real run, the Champion class handles merging/refreshing via
  // applyDebuff/applyEffect logic. For now, we print that it's working.
  // std::cout << "[Effect] Liandry's Suffering: Combat timer running..." <<
  // std::endl;
}
