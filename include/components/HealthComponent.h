#pragma once

class StatsComponent;

class HealthComponent {
public:
  HealthComponent(StatsComponent &stats);

  void takeDamage(float amount);
  void heal(float amount);

  void resetHealth();

  float getCurrentHealth() const;
  bool isAlive() const;

private:
  StatsComponent &statsRef;
  float currentHealth;
};
