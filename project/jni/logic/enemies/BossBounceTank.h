#ifndef BOSSBOUNCETANK_H_
#define BOSSBOUNCETANK_H_

#include "BounceTank.h"

class BossShieldTank : public BounceTank {
  public:
    BossShieldTank ()
      : BounceTank(BOSS_BCIRCLE_R) {
      setLives(3);
    }

    virtual eTankType getTankType () const { return BOSS_BOUNCE; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }
};

#endif /* BOSSBOUNCETANK_H_ */
