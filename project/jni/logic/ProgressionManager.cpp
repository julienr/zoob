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

size_t ProgressionManager::_level() const {
  return GameManager::getInstance()->getCurrentLevel();
}

bool ProgressionManager::hasBombs () const {
  return _level() > REWARD_BOMB;
}

bool ProgressionManager::hasShield () const {
  return _level() > REWARD_SHIELD;
}

void ProgressionManager::setPlayerForm (PlayerTank* player) const {
  if (_level() > BOUNCE_LVL)
    player->changePlayerForm(FORM_BOUNCE);
}

uint64_t ProgressionManager::getPlayerFireInterval () const {
  /*if (_level() <= FIRING_LVL)
    return 1000;
  else
    return 700;*/
  return 1000;
}

float ProgressionManager::getPlayerRocketsSpeed () const {
  if (_level() <= FIRING_LVL)
    return ROCKET_DEFAULT_SPEED;
  else
    return ROCKET_DEFAULT_SPEED*1.5f;
}

eReward ProgressionManager::getLastReward () const {
  const size_t l = _level();
  if (l == BOMB_LVL)
    return REWARD_BOMB;
  if (l == BOUNCE_LVL)
    return REWARD_BOUNCE;
  if (l == SHIELD_LVL)
    return REWARD_SHIELD;
  if (l == FIRING_LVL)
    return REWARD_FIRING;
  else
    return REWARD_NONE;
}

#define AV_FORM(f) availablePlayerForms.append(f)
void ProgressionManager::changedLevel () {
  availablePlayerForms.clear();
  AV_FORM(FORM_SIMPLE);
  if (_level() > BOUNCE_LVL)
    AV_FORM(FORM_BOUNCE);
}
