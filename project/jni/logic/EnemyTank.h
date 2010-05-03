#ifndef ENEMYTANK_H_
#define ENEMYTANK_H_

#include "Tank.h"
#include "Difficulty.h"
#include "ai/TankAI.h"

class EnemyTank: public Tank {
  public:
    EnemyTank(TankAI* ai)
      : Tank(new BurstFirePolicy(2000, 300, 3)),
        ai(ai),
        prepareFiring(false) {
    }

    virtual ~EnemyTank () {
      delete ai;
    }

    void prepareFire () {
      LOGE("prepareFire");
      prepareFiring = true;
      firingDelay = Difficulty::getInstance()->getFiringDelay();
    }

    void think (double elapsedS) {
      if (!prepareFiring)
        return;
      firingDelay -= elapsedS;
    }

    bool isPreparingFire () const {
      return prepareFiring;
    }

    double getFiringDelay () const {
      return firingDelay;
    }

    //indicate wether the tank is in a "firing streak" (for burst tank), so the delay
    //between fire will be handled by the firerate policy.
    //This will return false for all non-burst mode tanks
    bool isFiring () const {
      return getFireRatePolicy()->isFiring();
    }

    void cancelFiring () {
      return getFireRatePolicy()->cancelFiring();
    }

    bool fireReady () {
      const bool r = prepareFiring && firingDelay <= 0;
      if (r)
        prepareFiring = false;
      return r;
    }

    //Returns NULL for the player's tank
    TankAI* getAI () { return ai; }

  private:
    TankAI* ai;
    /**
     * Before the tank actually fire the rocket, it will have a delay during which it will
     * be in the firing state and provide some visual feedback (ie: growing) to the player so he
     * can react.
     */
    bool prepareFiring;
    double firingDelay;
};

#endif /* ENEMYTANK_H_ */
