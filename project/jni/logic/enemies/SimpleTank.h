#ifndef SIMPLETANK_H_
#define SIMPLETANK_H_

#include "ai/movement/PathPolicy.h"
#include "logic/EnemyTank.h"

class SimpleTank : public EnemyTank {
  public:
    SimpleTank (float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    SimpleTank (Path* p, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new PathPolicy())) {
      this->setPath(p);
    }

    eTankType getTankType () const { return TANK_SIMPLE; }
};

#endif /* REDTANK_H_ */
