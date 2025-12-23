#pragma once
#include "AutoAttackStrategy.h"
#include "Enums.h"
#include "components/HealthComponent.h"
#include "components/InventoryComponent.h"
#include "components/SpellbookComponent.h"
#include "components/StatsComponent.h"
#include "components/StatusComponent.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Champion {
public:
  Champion(const std::string &name, const BaseStats &baseData,
           std::unique_ptr<AutoAttackStrategy> strategy);

  StatsComponent &getStats() { return *stats; }
  const StatsComponent &getStats() const { return *stats; }

  HealthComponent &getHealth() { return *health; }
  const HealthComponent &getHealth() const { return *health; }

  InventoryComponent &getInventory() { return *inventory; }

  SpellbookComponent &getSpellbook() { return *spellbook; }
  const SpellbookComponent &getSpellbook() const { return *spellbook; }

  StatusComponent &getStatus() { return *status; }
  const StatusComponent &getStatus() const { return *status; }

  void setLevel(int newLevel);
  void addItem(const Item &item);

  void addAbility(const std::string &formName, AbilitySlot slot,
                  Ability ability);

  void addBonusStat(Stat stat, float value);

  void heal(float amount);
  void takeDamage(float damage);
  void consumeMana(float amount);
  void restoreMana(float amount);

  void useAbility(AbilitySlot slot);
  void castAbility(AbilitySlot slot, Champion &target);
  void resetAbilityCooldown(AbilitySlot slot);

  void applyDebuff(std::shared_ptr<Effect> debuff);
  void applyStatus(const std::string &status, float duration);
  bool hasStatus(const std::string &status) const;

  float performAutoAttack(Champion &target);

  void calculateTotalStats();
  void tick(float deltaTime);
  void printStats();

  std::string getName() const;
  int getLevel() const;

  const FinalStats &getTotalStats();
  const BaseStats &getBaseStatsData() const;
  const FinalStats &getBonusStats() const;

  const std::vector<std::shared_ptr<Effect>> &getEffects() const;
  const std::map<AbilitySlot, int> &getAbilityRanks() const;

  float getCurrentHealth() const;
  float getCurrentMana() const;
  bool isAlive() const;

  void setAbilityRanks(const std::map<AbilitySlot, int> &ranks);

private:
  std::string name;

  std::unique_ptr<StatsComponent> stats;
  std::unique_ptr<HealthComponent> health;
  std::unique_ptr<InventoryComponent> inventory;
  std::unique_ptr<SpellbookComponent> spellbook;
  std::unique_ptr<StatusComponent> status;

  std::unique_ptr<AutoAttackStrategy> autoAttackStrategy;
};
