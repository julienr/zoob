#ifndef SIMPLETANK_H_
#define SIMPLETANK_H_

#include "EnemyTank.h"
#include "ai/movement/PathPolicy.h"

class SimpleTank : public EnemyTank {
  public:
    SimpleTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    SimpleTank (Path* p)
      : EnemyTank (new TankAI(new AimPolicy(), new PathPolicy())) {
    }

    eTankType getTankType () const { return TANK_SIMPLE; }
};

#endif /* REDTANK_H_ */
