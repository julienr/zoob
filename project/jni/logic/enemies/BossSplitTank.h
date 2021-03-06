#ifndef BOSSSPLITTANK_H
#define BOSSSPLITTANK_H

#include "logic/EnemyTank.h"

class BossSplitTank : public ShieldTank {
  public:
    BossSplitTank (Path* p)
      : ShieldTank(BOSS_BCIRCLE_R, new TankAI(new AimPolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new SmartPolicy())) {
      setLives(3);
      setFirePolicy(new IntervalFirePolicy(1000));
      if (p) setPath(p);
    }

    eTankType getTankType () const { return BOSS_SPLIT; }

    double getInitialFiringDelay () const { return Difficulty::getInstance()->getBossFiringDelay(); }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, SPLIT);
    }
};

#endif /* BOSSSPLITTANK_H */
