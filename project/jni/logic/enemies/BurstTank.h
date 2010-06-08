#ifndef BURSTTANK_H_
#define BURSTTANK_H_

#include "ai/movement/PathPolicy.h"
#include "logic/EnemyTank.h"

#define BURST_INTERVAL 1000
#define IN_BURST_INTERVAL 200
#define NUM_BURST 3

class BurstTank : public EnemyTank {
  public:
    BurstTank (float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new StillPolicy()),
                   new BurstFirePolicy(Difficulty::getInstance()->getEnemiesFireInterval()/2, IN_BURST_INTERVAL, NUM_BURST),
                   Difficulty::getInstance()->getEnemiesFiringDelay()/2) {
    }

    BurstTank (Path* p, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new PathPolicy()),
                   new BurstFirePolicy(BURST_INTERVAL, IN_BURST_INTERVAL, NUM_BURST)) {
      this->setPath(p);
    }

    eTankType getTankType () const { return TANK_BURST; }
};

#endif /* BURSTTANK_H_ */
