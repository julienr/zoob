#ifndef BOSSSHIELDTANK_H_
#define BOSSSHIELDTANK_H_

#include "ShieldTank.h"

class BossShieldTank : public ShieldTank {
  public:
    BossShieldTank ()
      : ShieldTank(BOSS_BCIRCLE_R) {
      setLives(3);
    }

    virtual eTankType getTankType () const { return BOSS_SHIELD; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSSHIELDTANK_H_ */
