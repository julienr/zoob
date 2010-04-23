#ifndef SIMPLETANK_H_
#define SIMPLETANK_H_

#include "EnemyTank.h"

class SimpleTank : public EnemyTank {
  public:
    SimpleTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    eTankType getTankType () const { return TANK_SIMPLE; }
};

#endif /* REDTANK_H_ */
