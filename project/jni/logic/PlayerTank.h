#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "Tank.h"
#include "ProgressionManager.h"

class PlayerTank : public Tank {
  public:
    PlayerTank ()
      : Tank(new IntervalFirePolicy(1000)),
        currentForm (FORM_SIMPLE) {
    }

    eTankType getTankType () const { return TANK_PLAYER; }

    /* The player tank might switch between multiple "forms" (like simple, burst, shield).
     * These forms are all the same as some enemies tank. This method returns the current form
     */
    ePlayerForm getCurrentForm () const {
      return currentForm;
    }

    void changePlayerForm (ePlayerForm newForm) {
      ASSERT(ProgressionManager::getInstance()->getAvailablePlayerForms().contains(newForm));
      currentForm = newForm;
    }
  private:
    ePlayerForm currentForm;

};

#endif /* PLAYERTANK_H_ */
