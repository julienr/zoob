#ifndef BOSSSHIELDTANK_H_
#define BOSSSHIELDTANK_H_

#include "ShieldTank.h"

class BossShieldTank : public EnemyTank {
  public:
    BossShieldTank ()
      : EnemyTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(2000));
    }

    eTankType getTankType () const { return BOSS_SHIELD; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSSHIELDTANK_H_ */
