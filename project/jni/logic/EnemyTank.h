#ifndef ENEMYTANK_H_
#define ENEMYTANK_H_

#include "Tank.h"
#include "Difficulty.h"
#include "ai/TankAI.h"
#include "logic/Rocket.h"

class EnemyTank: public Tank {
  public:
    EnemyTank(float radius, TankAI* ai, FireRatePolicy* firePol=new IntervalFirePolicy(Difficulty::getInstance()->getEnemiesFireInterval()),
              double initialFiringDelay=Difficulty::getInstance()->getEnemiesFiringDelay())
      : Tank(radius, firePol),
        ai(ai),
        prepareFiring(false),
        initialFiringDelay(initialFiringDelay){
    }

    virtual ~EnemyTank () {
      delete ai;
    }

    eTankCategory getTankCategory () const {
      return CAT_AI;
    }

    void prepareFire () {
      prepareFiring = true;
      firingDelay = initialFiringDelay;
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
      prepareFiring = false;
      getFireRatePolicy()->cancelFiring();
    }

    bool fireReady () {
      const bool r = prepareFiring && firingDelay <= 0;
      if (r)
        prepareFiring = false;
      return r;
    }

    bool explode (Entity* e, const Vector2& colPoint) {
      //Avoid a tank being exploded by his friends rockets
      if (e && e->getType() == ENTITY_ROCKET) {
        Rocket* r = static_cast<Rocket*>(e);
        //Enemy tanks can only die because of player rockets
        if (r->getOwner()->getTankType() != TANK_PLAYER)
          return false;
      }
      return Tank::explode(e, colPoint);
    }

    //Returns NULL for the player's tank
    TankAI* getAI () const { return ai; }

    virtual Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, Difficulty::getInstance()->getEnemiesRocketSpeed(), BOUNCE);
    }

    double getInitialFiringDelay () const { return initialFiringDelay; }
  private:
    TankAI* ai;
    /**
     * Before the tank actually fire the rocket, it will have a delay during which it will
     * be in the firing state and provide some visual feedback (ie: growing) to the player so he
     * can react.
     */
    //FIXME: move that to a policy
    bool prepareFiring;
    double firingDelay;
    const double initialFiringDelay;
};

#endif /* ENEMYTANK_H_ */
