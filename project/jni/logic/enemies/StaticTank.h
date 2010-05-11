#ifndef STATICTANK_H_
#define STATICTANK_H_

#include "logic/EnemyTank.h"
#include "ai/SmartPolicy.h"

class StaticTank : public EnemyTank {
  public:
    StaticTank (SmartPolicy* ai = new SmartPolicy())
      : EnemyTank (TANK_BCIRCLE_R, new TankAI(ai, ai)/*new TankAI(new NonePolicy(), new StillPolicy())*/) {
    }

    eTankType getTankType () const { return TANK_STATIC; }
};

#endif /* STATICTANK_H_ */
