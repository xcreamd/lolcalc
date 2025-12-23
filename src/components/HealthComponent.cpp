#include "HealthComponent.h"
#include "StatsComponent.h"
#include <algorithm>

HealthComponent::HealthComponent(StatsComponent &stats) : statsRef(stats) {
  resetHealth();
}

void HealthComponent::takeDamage(float amount) {
  // Future: Shield logic goes here
  currentHealth -= amount;
  if (currentHealth < 0)
    currentHealth = 0;
}

void HealthComponent::heal(float amount) {
  currentHealth += amount;
  float maxHealth = statsRef.getTotalStats().health;
  if (currentHealth > maxHealth)
    currentHealth = maxHealth;
}

void HealthComponent::resetHealth() {
  currentHealth = statsRef.getTotalStats().health;
}

float HealthComponent::getCurrentHealth() const { return currentHealth; }

bool HealthComponent::isAlive() const { return currentHealth > 0; }
