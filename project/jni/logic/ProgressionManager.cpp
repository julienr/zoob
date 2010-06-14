#include "ProgressionManager.h"
#include "view/GameManager.h"

ProgressionManager* ProgressionManager::instance = NULL;

ProgressionManager::ProgressionManager ()
  : availablePlayerForms(3) {
  availablePlayerForms.append(FORM_SIMPLE);
}

size_t ProgressionManager::_level() const {
  return GameManager::getInstance()->getCurrentLevel();
}

bool ProgressionManager::hasBombs () const {
  if (_level() <= 5)
    return false;
  else
    return true;
}

bool ProgressionManager::hasShield () const {
  if (_level() <= 25)
    return false;
  else
    return true;
}

eReward ProgressionManager::getLastReward () const {
  const size_t l = _level();
  if (l == 5)
    return REWARD_BOMB;
  if (l == 15)
    return REWARD_BOUNCE;
  if (l == 25)
    return REWARD_SHIELD;
  if (l == 35)
    return REWARD_FIRING;
  else
    return REWARD_NONE;
}

#define AV_FORM(f) availablePlayerForms.append(f)
void ProgressionManager::changedLevel () {
  availablePlayerForms.clear();
  AV_FORM(FORM_SIMPLE);
  if (_level() <= 5) {
  } else {
    AV_FORM(FORM_BOUNCE);
    AV_FORM(FORM_BURST);
  }
}
