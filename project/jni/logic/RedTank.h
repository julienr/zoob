#ifndef REDTANK_H_
#define REDTANK_H_

#include "EnemyTank.h"

class RedTank : public EnemyTank {
  public:
    RedTank ()
      : EnemyTank (new TankAI(new NonePolicy(), new StillPolicy())) {
    }

    eColor getColor () const { return RED; }
};

#endif /* REDTANK_H_ */
