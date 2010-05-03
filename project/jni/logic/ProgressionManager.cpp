#include "ProgressionManager.h"

ProgressionManager* ProgressionManager::instance = NULL;

ProgressionManager::ProgressionManager ()
  : availablePlayerForms(3) {
  availablePlayerForms.append(FORM_SIMPLE);
  availablePlayerForms.append(FORM_SHIELD);
  availablePlayerForms.append(FORM_BURST);
}
