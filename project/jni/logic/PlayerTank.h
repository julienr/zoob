#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "Tank.h"
#include "ProgressionManager.h"
#include "logic/Rocket.h"
#include "lib/Timer.h"
#include "Difficulty.h"

/*#define PLAYER_FIRE_INTERVAL 1000*/
#define PLAYER_BURST_INTERVAL 1500
#define PLAYER_IN_BURST_INTERVAL 200
#define PLAYER_NUM_BURSTS 3

#define PLAYER_SHIELD_TIME 10

class PlayerTank : public Tank {
  public:
    PlayerTank ()
      : Tank(TANK_BCIRCLE_R, new IntervalFirePolicy(ProgressionManager::getInstance()->getPlayerFireInterval())),
        currentForm (FORM_SIMPLE),
        shieldTimer(PLAYER_SHIELD_TIME) {
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
      if (shieldTimer.isActive()) {
        if (e->getType() == ENTITY_ROCKET) {
          Rocket* r = static_cast<Rocket*>(e);
          if (r->getNumBounces() < Difficulty::getInstance()->getPlayerShieldResistance())
            return false;
        }
      } else {
        //Avoid a tank being exploded by his friends rockets
        if (e && e->getType() == ENTITY_ROCKET) {
          Rocket* r = static_cast<Rocket*>(e);
          //Player can only die of enemies rockets
          if (r->getOwner()->getTankType() == TANK_PLAYER)
            return false;
        }
      }
      return Tank::explode(e, colPoint);
    }

    void startShield () {
      shieldTimer.start();
    }

    double getShieldTimeLeft () const {
      return shieldTimer.getTimeLeft();
    }

    void changePlayerForm (ePlayerForm newForm);

    bool bounce (Entity* e, const Vector2& colPoint) {
      if (currentForm == FORM_BOUNCE)
        return shieldBounce(e, colPoint);
      else
        return false;
    }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, BOUNCE, ProgressionManager::getInstance()->getPlayerRocketsSpeed());
    }
  private:
    ePlayerForm currentForm;

    Timer shieldTimer;

};

#endif /* PLAYERTANK_H_ */
