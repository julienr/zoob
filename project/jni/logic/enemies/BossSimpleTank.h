#ifndef BOSSSIMPLETANK_H_
#define BOSSSIMPLETANK_H_

#include "logic/EnemyTank.h"

class BossSimpleTank : public EnemyTank {
  public:
    BossSimpleTank (Path* p=NULL)
      : EnemyTank(BOSS_BCIRCLE_R, new TankAI(/*new AimPolicy()*/new NonePolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(1000));
      if (p) setPath(p);
    }

    eTankType getTankType () const { return BOSS_SIMPLE; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSSIMPLETANK_H_ */
