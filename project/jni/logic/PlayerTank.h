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

class PlayerTank : public Tank {
  public:
    PlayerTank (FireRatePolicy* pol)
      : Tank(TANK_BCIRCLE_R, pol),
        currentForm (FORM_SIMPLE) {
      setLives(3);
    }

    eTankType getTankType () const { return TANK_PLAYER; }

    eTankCategory getTankCategory () const {
      return CAT_PLAYER;
    }

    /* The player tank might switch between multiple "forms" (like simple, burst, shield).
     * These forms are all the same as some enemies tank. This method returns the current form
     */
    ePlayerForm getCurrentForm () const {
      return currentForm;
    }

    int explode (Entity* e, const Vector2& colPoint);

    void changePlayerForm (ePlayerForm newForm);

    bool bounce (Entity* e, const Vector2& colPoint) {
      if (currentForm == FORM_BOUNCE)
        return shieldBounce(e, colPoint);
      else
        return false;
    }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, ProgressionManager::getInstance()->getPlayerRocketsSpeed(), BOUNCE);
    }
  private:
    ePlayerForm currentForm;
};

#endif /* PLAYERTANK_H_ */
