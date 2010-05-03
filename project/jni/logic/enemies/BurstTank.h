#ifndef BURSTTANK_H_
#define BURSTTANK_H_

#include "ai/movement/PathPolicy.h"
#include "logic/EnemyTank.h"

#define BURST_INTERVAL 2000
#define IN_BURST_INTERVAL 200
#define NUM_BURST 3

class BurstTank : public EnemyTank {
  public:
    BurstTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy()),
                   new BurstFirePolicy(BURST_INTERVAL, IN_BURST_INTERVAL, NUM_BURST)) {
    }

    BurstTank (Path* p)
      : EnemyTank (new TankAI(new AimPolicy(), new PathPolicy()),
                   new BurstFirePolicy(BURST_INTERVAL, IN_BURST_INTERVAL, NUM_BURST)) {
      this->setPath(p);
    }

    eTankType getTankType () const { return TANK_BURST; }
};

#endif /* BURSTTANK_H_ */
