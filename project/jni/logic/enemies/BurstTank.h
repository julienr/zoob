#ifndef BURSTTANK_H_
#define BURSTTANK_H_

#include "ai/movement/PathPolicy.h"
#include "logic/EnemyTank.h"

#define IN_BURST_INTERVAL 350
#define NUM_BURST 3

class BurstTank : public EnemyTank {
  public:
    BurstTank (Path* p, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new StillPolicy()),
                   new BurstFirePolicy(Difficulty::getInstance()->getEnemiesFireInterval()/2, IN_BURST_INTERVAL, NUM_BURST),
                   Difficulty::getInstance()->getEnemiesFiringDelay()/2) {
      if (p) this->setPath(p);
    }

    eTankType getTankType () const { return TANK_BURST; }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, Difficulty::getInstance()->getEnemiesBurstRocketSpeed(), BOUNCE);
    }
};

#endif /* BURSTTANK_H_ */
