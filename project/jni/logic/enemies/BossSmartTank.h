#ifndef BOSSSMARTTANK_H_
#define BOSSSMARTTANK_H_

#include "logic/EnemyTank.h"

class BossSmartTank : public ShieldTank {
  public:
    BossSmartTank ()
      : ShieldTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(1000));
    }

    eTankType getTankType () const { return BOSS_SMART; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, SPLIT);
    }
};

#endif /* BOSSSMARTTANK_H_ */
