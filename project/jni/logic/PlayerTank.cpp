#include "PlayerTank.h"

void PlayerTank::changePlayerForm(ePlayerForm newForm) {
  //ASSERT(ProgressionManager::getInstance()->getAvailablePlayerForms().contains(newForm));
  currentForm = newForm;
  switch (newForm) {
    case FORM_SIMPLE:
    case FORM_BOUNCE:
      setFirePolicy(new IntervalFirePolicy(ProgressionManager::getInstance()->getPlayerFireInterval()));
      break;
    case FORM_BURST:
      setFirePolicy(new BurstFirePolicy(PLAYER_BURST_INTERVAL, PLAYER_IN_BURST_INTERVAL, PLAYER_NUM_BURSTS));
      break;
  }
}

int PlayerTank::explode (Entity* e, const Vector2& colPoint) {
  if (shieldActive()) {
    if (e->getType() == ENTITY_ROCKET) {
      Rocket* r = static_cast<Rocket*> (e);
      if (r->getNumBounces() < Difficulty::getInstance()->getPlayerShieldResistance())
        return 0;
    }
  } else {
    //Avoid a tank being exploded by his own rockets
    if (e && e->getType() == ENTITY_ROCKET) {
      Rocket* r = static_cast<Rocket*> (e);
      if (r->getOwner() == this)
        return 0;
    }
  }
  return Tank::explode(e, colPoint);
}