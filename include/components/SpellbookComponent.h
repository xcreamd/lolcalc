#pragma once
#include "Ability.h"
#include "Enums.h"
#include <map>
#include <string>
#include <vector>

class StatsComponent;

class SpellbookComponent {
public:
  SpellbookComponent(StatsComponent &stats);

  // Adds an ability to a specific form (e.g. "Hammer" -> Q -> ToTheSkies)
  // Use "Default" for standard champions.
  void addAbility(const std::string &formName, AbilitySlot slot,
                  Ability ability);

  // Switch the active set of abilities (e.g. Jayce R)
  void setForm(const std::string &formName);
  std::string getCurrentForm() const;

  // Gets ability from the CURRENT ACTIVE form
  Ability *getAbility(AbilitySlot slot);

  void setAbilityRanks(const std::map<AbilitySlot, int> &ranks);
  const std::map<AbilitySlot, int> &getAbilityRanks() const;

  // Resource Management
  void consumeMana(float amount);
  void restoreMana(float amount);
  void resetMana();
  float getCurrentMana() const;

  // Updates cooldowns for ALL abilities in ALL forms
  void tick(float deltaTime);

  // Resets cooldown for the specific slot in the active form
  void resetCooldown(AbilitySlot slot);

  // Accessor for the active form's abilities (useful for tick/display)
  const std::map<AbilitySlot, Ability> &getAbilities() const;

private:
  StatsComponent &statsRef;

  // Nested map: Form Name -> (Slot -> Ability)
  // Example: forms["MercuryHammer"][AbilitySlot::Q]
  std::map<std::string, std::map<AbilitySlot, Ability>> forms;

  std::string activeForm = "Default";

  std::map<AbilitySlot, int> abilityRanks;
  float currentMana;
};
