#ifndef BOSSBOUNCETANK_H_
#define BOSSBOUNCETANK_H_

#include "BounceTank.h"


class BossBounceTank : public EnemyTank {
  public:
    BossBounceTank ()
      : EnemyTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(2000));
    }

    eTankType getTankType () const { return BOSS_BOUNCE; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }

    bool bounce (Entity* e, const Vector2& colPoint) {
      return shieldBounce(e, colPoint);
    }
};

#endif /* BOSSBOUNCETANK_H_ */
