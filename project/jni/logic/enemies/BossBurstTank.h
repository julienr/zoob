#ifndef BOSSBURSTTANK_H_
#define BOSSBURSTTANK_H_

#include "BurstTank.h"

class BossBurstTank : public EnemyTank {
  public:
    BossBurstTank ()
      : EnemyTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new BurstFirePolicy(Difficulty::getInstance()->getEnemiesFireInterval()/2, IN_BURST_INTERVAL, NUM_BURST));
    }

    eTankType getTankType () const { return BOSS_BURST; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }

    bool bounce (Entity* e, const Vector2& colPoint) {
      return shieldBounce(e, colPoint);
    }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, BOUNCE, BURST_ROCKET_SPEED);
    }
};


#endif /* BOSSBURSTTANK_H_ */
