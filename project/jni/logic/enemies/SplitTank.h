#ifndef SPLITTANK_H_
#define SPLITTANK_H_

#include "ai/movement/PathPolicy.h"
#include "ShieldTank.h"
#include "ai/shoot/AlwaysPolicy.h"

class SplitTank: public ShieldTank {
  public:
    SplitTank (Path* p, float radius=TANK_BCIRCLE_R)
      : ShieldTank (radius, new TankAI(new AlwaysPolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new StillPolicy())) {
      if (p) this->setPath(p);
    }

    eTankType getTankType () const { return TANK_SPLIT; }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, Difficulty::getInstance()->getEnemiesRocketSpeed(), SPLIT);
    }
};

#endif /* SPLITTANK_H_ */
