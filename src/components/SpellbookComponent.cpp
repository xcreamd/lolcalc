#include "components/SpellbookComponent.h"
#include "components/StatsComponent.h"
#include <iostream>

SpellbookComponent::SpellbookComponent(StatsComponent &stats)
    : statsRef(stats) {
  resetMana();
}

void SpellbookComponent::addAbility(const std::string &formName,
                                    AbilitySlot slot, Ability ability) {
  forms[formName].emplace(slot, ability);
}

void SpellbookComponent::setForm(const std::string &formName) {
  if (forms.count(formName)) {
    activeForm = formName;
    std::cout << "[Spellbook] Switched to form: " << formName << std::endl;
  } else {
    std::cerr << "[Spellbook] Error: Attempted to switch to unknown form '"
              << formName << "'" << std::endl;
  }
}

std::string SpellbookComponent::getCurrentForm() const { return activeForm; }

Ability *SpellbookComponent::getAbility(AbilitySlot slot) {
  // Look up ability in the current active form
  if (forms.count(activeForm)) {
    auto &currentSet = forms[activeForm];
    auto it = currentSet.find(slot);
    if (it != currentSet.end()) {
      return &it->second;
    }
  }
  return nullptr;
}

void SpellbookComponent::setAbilityRanks(
    const std::map<AbilitySlot, int> &ranks) {
  abilityRanks = ranks;
}

const std::map<AbilitySlot, int> &SpellbookComponent::getAbilityRanks() const {
  return abilityRanks;
}

void SpellbookComponent::consumeMana(float amount) {
  currentMana -= amount;
  if (currentMana < 0)
    currentMana = 0;
}

void SpellbookComponent::restoreMana(float amount) {
  currentMana += amount;
  float maxMana = statsRef.getTotalStats().mana;
  if (currentMana > maxMana)
    currentMana = maxMana;
}

void SpellbookComponent::resetMana() {
  currentMana = statsRef.getTotalStats().mana;
}

float SpellbookComponent::getCurrentMana() const { return currentMana; }

void SpellbookComponent::tick(float deltaTime) {
  // Tick ALL abilities in ALL forms (cooldowns usually run in background even
  // if form is inactive)
  for (auto &[formName, abilitySet] : forms) {
    for (auto &[slot, ability] : abilitySet) {
      ability.tick(deltaTime);
    }
  }
}

void SpellbookComponent::resetCooldown(AbilitySlot slot) {
  // Resets cooldown for the ability in the ACTIVE form
  if (Ability *ability = getAbility(slot)) {
    ability->resetCooldown();
  }
}

const std::map<AbilitySlot, Ability> &SpellbookComponent::getAbilities() const {
  static std::map<AbilitySlot, Ability> empty;
  if (forms.count(activeForm)) {
    return forms.at(activeForm);
  }
  return empty;
}
