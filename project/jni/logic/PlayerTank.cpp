#include "PlayerTank.h"

void PlayerTank::changePlayerForm(ePlayerForm newForm) {
  ASSERT(ProgressionManager::getInstance()->getAvailablePlayerForms().contains(newForm));
  currentForm = newForm;
  switch (newForm) {
    case FORM_SIMPLE:
    case FORM_BOUNCE:
      setFirePolicy(new IntervalFirePolicy(PLAYER_FIRE_INTERVAL));
      break;
    case FORM_BURST:
      setFirePolicy(new BurstFirePolicy(PLAYER_BURST_INTERVAL, PLAYER_IN_BURST_INTERVAL, PLAYER_NUM_BURSTS));
      break;
  }
}
