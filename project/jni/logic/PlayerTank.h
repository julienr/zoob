#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "Tank.h"
#include "ProgressionManager.h"
#include "logic/Rocket.h"

#define PLAYER_FIRE_INTERVAL 1000
#define PLAYER_BURST_INTERVAL 1500
#define PLAYER_IN_BURST_INTERVAL 200
#define PLAYER_NUM_BURSTS 3

class PlayerTank : public Tank {
  public:
    PlayerTank ()
      : Tank(TANK_BCIRCLE_R, new IntervalFirePolicy(PLAYER_FIRE_INTERVAL)),
        currentForm (FORM_SIMPLE) {
      setLives(3);
    }

    eTankType getTankType () const { return TANK_PLAYER; }

    /* The player tank might switch between multiple "forms" (like simple, burst, shield).
     * These forms are all the same as some enemies tank. This method returns the current form
     */
    ePlayerForm getCurrentForm () const {
      return currentForm;
    }

    bool explode (Entity* e, const Vector2& colPoint) {
      //Avoid a tank being exploded by his friends rockets
      if (e && e->getType() == ENTITY_ROCKET) {
        Rocket* r = static_cast<Rocket*>(e);
        //Player can only die of enemies rockets
        if (r->getOwner()->getTankType() == TANK_PLAYER)
          return false;
      }
      return Tank::explode(e, colPoint);
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
