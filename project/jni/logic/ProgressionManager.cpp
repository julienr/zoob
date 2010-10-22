#include "ProgressionManager.h"
#include "view/GameManager.h"
#include "logic/PlayerTank.h"

//the levels at which the different rewards are given
#define BOMB_LVL 5
#define BOUNCE_LVL 15
#define SHIELD_LVL 25
#define FIRING_LVL 35

ProgressionManager* ProgressionManager::instance = NULL;

ProgressionManager::ProgressionManager ()
  : availablePlayerForms(3) {
  availablePlayerForms.append(FORM_SIMPLE);
  memset(items, 0, sizeof(uint8_t)*4);
}

const Level* ProgressionManager::_level() const {
  return Game::getInstance()->getLevel();
}

bool ProgressionManager::hasBombs () const {
  return items[0];
}

bool ProgressionManager::hasShield () const {
  return items[1];
}

void ProgressionManager::setPlayerForm (Level* UNUSED(level), PlayerTank* player) const {
  if (items[2])
    player->changePlayerForm(FORM_BOUNCE);
}

uint64_t ProgressionManager::getPlayerFireInterval () const {
  return 1000;
}

float ProgressionManager::getPlayerRocketsSpeed () const {
  if (items[3])
    return 3.0f;
  else
    return 2.0f;
}

eReward ProgressionManager::getLastReward () const {
  return _level()->getReward();
}

#define AV_FORM(f) availablePlayerForms.append(f)
void ProgressionManager::changedLevel (const Level* newLevel) {
  availablePlayerForms.clear();
  AV_FORM(FORM_SIMPLE);
  if (newLevel->hasItem(ITEM_BOUNCE))
    AV_FORM(FORM_BOUNCE);

  items[0] = newLevel->hasItem(ITEM_BOMB);
  items[1] = newLevel->hasItem(ITEM_SHIELD);
  items[2] = newLevel->hasItem(ITEM_BOUNCE);
  items[3] = newLevel->hasItem(ITEM_FIRING);
  LOGE("bombs : %i, shield : %i, bounce : %i, firing : %i, reward : %i", items[0], items[1], items[2], items[3], newLevel->getReward());
}
