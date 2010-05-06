#ifndef BOSSSIMPLETANK_H_
#define BOSSSIMPLETANK_H_

#include "SimpleTank.h"

class BossSimpleTank : public SimpleTank {
  public:
    BossSimpleTank ()
      : SimpleTank(BOSS_BCIRCLE_R) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(1000));
    }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSSIMPLETANK_H_ */
