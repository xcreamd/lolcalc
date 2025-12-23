#include "Champion.h"
#include "DamageContext.h"
#include <iostream>

Champion::Champion(const std::string &name, const BaseStats &baseData,
                   std::unique_ptr<AutoAttackStrategy> strategy)
    : name(name), autoAttackStrategy(std::move(strategy)) {

  stats = std::make_unique<StatsComponent>(baseData);
  health = std::make_unique<HealthComponent>(*stats);
  inventory = std::make_unique<InventoryComponent>();
  spellbook = std::make_unique<SpellbookComponent>(*stats);
  status = std::make_unique<StatusComponent>();

  setLevel(1);
}

void Champion::setLevel(int newLevel) {
  stats->setLevel(newLevel);
  calculateTotalStats();
  health->resetHealth();
  spellbook->resetMana();
}

void Champion::addItem(const Item &item) {
  inventory->addItem(item);

  if (item.getEffect()) {
    status->addEffect(item.getEffect());
  }

  calculateTotalStats();
  health->resetHealth();
  spellbook->resetMana();
}

void Champion::addAbility(const std::string &formName, AbilitySlot slot,
                          Ability ability) {
  spellbook->addAbility(formName, slot, ability);

  if (ability.getEffect()) {
    status->addEffect(ability.getEffect());
  }
  calculateTotalStats();
}

void Champion::addBonusStat(Stat stat, float value) {
  stats->addBonusStat(stat, value);
}

void Champion::takeDamage(float damage) {
  float modifiedDamage = damage;
  for (const auto &effect : status->getEffects()) {
    modifiedDamage = effect->onDamageTaken(modifiedDamage);
  }
  health->takeDamage(modifiedDamage);
}

void Champion::consumeMana(float amount) { spellbook->consumeMana(amount); }

void Champion::restoreMana(float amount) { spellbook->restoreMana(amount); }

void Champion::useAbility(AbilitySlot slot) {
  Ability *ability = spellbook->getAbility(slot);
  if (ability) {
    int rank = 1;
    if (spellbook->getAbilityRanks().count(slot))
      rank = spellbook->getAbilityRanks().at(slot);

    if (ability->canCast(stats->getTotalStats(), spellbook->getCurrentMana(),
                         rank)) {
      consumeMana(ability->getResourceCost(rank));
      ability->putOnCooldown(stats->getTotalStats(), rank);
      ability->activate();

      for (const auto &effect : status->getEffects()) {
        effect->onCast(*this, slot);
      }

      calculateTotalStats();
    }
  }
}

void Champion::castAbility(AbilitySlot slot, Champion &target) {
  Ability *ability = spellbook->getAbility(slot);
  if (!ability)
    return;

  int rank = 1;
  if (spellbook->getAbilityRanks().count(slot))
    rank = spellbook->getAbilityRanks().at(slot);

  if (!ability->canCast(stats->getTotalStats(), spellbook->getCurrentMana(),
                        rank)) {
    return;
  }

  consumeMana(ability->getResourceCost(rank));
  ability->putOnCooldown(stats->getTotalStats(), rank);

  float rawDamage = ability->calculateDamage(
      rank, stats->getTotalStats(), stats->getBaseStatsData(), *this, target);
  DamageType type = ability->getDamageType();

  float finalDamage = 0;
  if (type == DamageType::Physical) {
    float targetArmor = target.getTotalStats().armor;
    finalDamage = rawDamage * (100.0f / (100.0f + targetArmor));
  } else if (type == DamageType::Magic) {
    float targetMR = target.getTotalStats().magicResist;
    finalDamage = rawDamage * (100.0f / (100.0f + targetMR));
  } else {
    finalDamage = rawDamage;
  }

  std::string slotName = "Ability";
  if (slot == AbilitySlot::Q)
    slotName = "Q";
  else if (slot == AbilitySlot::W)
    slotName = "W";
  else if (slot == AbilitySlot::E)
    slotName = "E";
  else if (slot == AbilitySlot::R)
    slotName = "R";

  std::cout << name << " casts " << ability->getName() << " (" << slotName
            << ") on " << target.getName() << " for " << finalDamage
            << " damage." << std::endl;

  target.takeDamage(finalDamage);

  for (const auto &effect : status->getEffects()) {
    effect->onSpellHit(*this, target);
  }

  for (const auto &effect : status->getEffects()) {
    effect->onCast(*this, slot);
  }

  if (!ability->getStatusToApply().empty()) {
    target.applyStatus(ability->getStatusToApply(),
                       ability->getStatusDuration());
  }
}

void Champion::resetAbilityCooldown(AbilitySlot slot) {
  spellbook->resetCooldown(slot);
  std::cout << "[Effect] Cooldown reset for ability slot " << (int)slot
            << std::endl;
}

void Champion::applyDebuff(std::shared_ptr<Effect> debuff) {
  status->applyDebuff(debuff, name);
}

void Champion::applyStatus(const std::string &statusName, float duration) {
  status->applyStatus(statusName, duration, name);
}

bool Champion::hasStatus(const std::string &statusName) const {
  return status->hasStatus(statusName);
}

float Champion::performAutoAttack(Champion &target) {
  DamageContext ctx;
  ctx.source = this;
  ctx.target = &target;
  ctx.abilityName = "AutoAttack";
  ctx.isBasicAttack = true;

  float damage = autoAttackStrategy->execute(ctx);

  for (const auto &effect : status->getEffects()) {
    effect->onAutoAttack(*this, target);
  }

  calculateTotalStats();
  std::cout << name << " attacks " << target.getName() << " for " << damage
            << " total damage." << std::endl;
  return damage;
}

void Champion::tick(float deltaTime) {
  spellbook->tick(deltaTime);
  status->tick(*this, deltaTime);
  calculateTotalStats();
}

void Champion::calculateTotalStats() {
  stats->clearBonuses();

  for (const auto &effect : status->getEffects()) {
    effect->applyPassiveStats(*this);
  }

  for (const auto &item : inventory->getItems()) {
    for (const auto &statPair : item.getStats()) {
      stats->addBonusStat(statPair.first, statPair.second);
    }
  }

  for (const auto &effect : status->getEffects()) {
    effect->applyScalingStats(*this);
  }

  stats->calculateTotalStats();
}

void Champion::printStats() {
  FinalStats s = stats->getTotalStats();
  std::cout << "--- " << name << "'s Stats ---" << std::endl;
  std::cout << "Level: " << stats->getLevel() << std::endl;
  std::cout << "Health: " << s.health << std::endl;
  std::cout << "Mana: " << s.mana
            << " (Current: " << spellbook->getCurrentMana() << ")" << std::endl;
  std::cout << "Attack Damage: " << s.attackDamage << std::endl;
  std::cout << "Ability Power: " << s.abilityPower << std::endl;
  std::cout << "Attack Speed: " << s.attackSpeed << std::endl;
  std::cout << "Crit Chance: " << s.criticalStrikeChance * 100 << "%"
            << std::endl;
  std::cout << "Crit Damage: " << s.criticalStrikeDamage * 100 << "%"
            << std::endl;
  std::cout << "Ability Haste: " << s.abilityHaste << std::endl;
  std::cout << "Armor: " << s.armor << std::endl;
  std::cout << "Magic Resist: " << s.magicResist << std::endl;
  std::cout << "--------------------" << std::endl << std::endl;
}

// void Champion::gatherEffects() {
//
//
//
// }

std::string Champion::getName() const { return name; }
int Champion::getLevel() const { return stats->getLevel(); }

const FinalStats &Champion::getTotalStats() { return stats->getTotalStats(); }
const BaseStats &Champion::getBaseStatsData() const {
  return stats->getBaseStatsData();
}

const FinalStats &Champion::getBonusStats() const {

  return stats->getTotalStats();
}

const std::vector<std::shared_ptr<Effect>> &Champion::getEffects() const {
  return status->getEffects();
}
const std::map<AbilitySlot, int> &Champion::getAbilityRanks() const {
  return spellbook->getAbilityRanks();
}
float Champion::getCurrentHealth() const { return health->getCurrentHealth(); }
float Champion::getCurrentMana() const { return spellbook->getCurrentMana(); }
bool Champion::isAlive() const { return health->isAlive(); }
void Champion::setAbilityRanks(const std::map<AbilitySlot, int> &ranks) {
  spellbook->setAbilityRanks(ranks);
}
