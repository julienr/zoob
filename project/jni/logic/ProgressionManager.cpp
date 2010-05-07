#include "ProgressionManager.h"
#include "view/GameManager.h"

ProgressionManager* ProgressionManager::instance = NULL;

ProgressionManager::ProgressionManager ()
  : availablePlayerForms(3) {
  availablePlayerForms.append(FORM_SIMPLE);
}

size_t ProgressionManager::_level() {
  return GameManager::getInstance()->getCurrentLevel();
}

bool ProgressionManager::hasBombs () {
  if (_level() <= 5)
    return false;
  else
    return true;
}

#define AV_FORM(f) availablePlayerForms.append(f)
void ProgressionManager::changedLevel () {
  availablePlayerForms.clear();
  AV_FORM(FORM_SIMPLE);
  if (_level() <= 5) {
  } else {
    AV_FORM(FORM_SHIELD);
    AV_FORM(FORM_BURST);
  }
}
