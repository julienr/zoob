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
}

const Level* ProgressionManager::_level() const {
  LOGE("_level(), game instance %i", Game::getInstance());
  return Game::getInstance()->getLevel();
}

bool ProgressionManager::hasBombs () const {
  return _level()->hasItem(ITEM_BOMB);
}

bool ProgressionManager::hasShield () const {
  return _level()->hasItem(ITEM_SHIELD);
}

void ProgressionManager::setPlayerForm (Level* level, PlayerTank* player) const {
  if (level->hasItem(ITEM_BOUNCE))
    player->changePlayerForm(FORM_BOUNCE);
}

uint64_t ProgressionManager::getPlayerFireInterval () const {
  return 1000;
}

float ProgressionManager::getPlayerRocketsSpeed () const {
  if (_level()->hasItem(ITEM_FIRING))
    return 3.0f;
  else
    return 2.0f;
}

eReward ProgressionManager::getLastReward () const {
  return _level()->getReward();
}

#define AV_FORM(f) availablePlayerForms.append(f)
void ProgressionManager::changedLevel () {
  availablePlayerForms.clear();
  AV_FORM(FORM_SIMPLE);
  if (_level()->hasItem(ITEM_BOUNCE))
    AV_FORM(FORM_BOUNCE);
}
