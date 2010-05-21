#ifndef BOSSSIMPLETANK_H_
#define BOSSSIMPLETANK_H_

#include "logic/EnemyTank.h"

class BossSimpleTank : public EnemyTank {
  public:
    BossSimpleTank ()
      : EnemyTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(1000));
    }

    eTankType getTankType () const { return BOSS_SIMPLE; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSSIMPLETANK_H_ */
