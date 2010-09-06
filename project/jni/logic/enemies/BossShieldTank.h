#ifndef BOSSSHIELDTANK_H_
#define BOSSSHIELDTANK_H_

#include "ShieldTank.h"

class BossShieldTank : public ShieldTank {
  public:
    BossShieldTank (Path* p)
      : ShieldTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(2000));
      if (p) setPath(p);
    }

    eTankType getTankType () const { return BOSS_SHIELD; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }


};

#endif /* BOSSSHIELDTANK_H_ */
