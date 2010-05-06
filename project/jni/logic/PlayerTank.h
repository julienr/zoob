#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "Tank.h"
#include "ProgressionManager.h"

#define PLAYER_FIRE_INTERVAL 1000
#define PLAYER_BURST_INTERVAL 1500
#define PLAYER_IN_BURST_INTERVAL 200
#define PLAYER_NUM_BURSTS 3

class PlayerTank : public Tank {
  public:
    PlayerTank ()
      : Tank(TANK_BCIRCLE_R, new IntervalFirePolicy(PLAYER_FIRE_INTERVAL)),
        currentForm (FORM_SIMPLE) {
    }

    eTankType getTankType () const { return TANK_PLAYER; }

    /* The player tank might switch between multiple "forms" (like simple, burst, shield).
     * These forms are all the same as some enemies tank. This method returns the current form
     */
    ePlayerForm getCurrentForm () const {
      return currentForm;
    }

    void changePlayerForm (ePlayerForm newForm);

    bool bounce (Entity* e, const Vector2& colPoint) {
      if (currentForm == FORM_SHIELD)
        return shieldBounce(e, colPoint);
      else
        return false;
    }
  private:
    ePlayerForm currentForm;

};

#endif /* PLAYERTANK_H_ */
