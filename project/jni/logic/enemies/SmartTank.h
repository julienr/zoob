#ifndef SMARTTANK_H_
#define SMARTTANK_H_

#include "ai/movement/PathPolicy.h"
#include "ShieldTank.h"
#include "ai/shoot/AlwaysPolicy.h"

class SmartTank: public ShieldTank {
  public:
    SmartTank (float radius=TANK_BCIRCLE_R)
      : ShieldTank (radius, new TankAI(new AlwaysPolicy(), new StillPolicy())) {
    }

    SmartTank (Path* p, float radius=TANK_BCIRCLE_R)
      : ShieldTank (radius, new TankAI(new AlwaysPolicy(), new StillPolicy())) {
      this->setPath(p);
    }

    eTankType getTankType () const { return TANK_SMART; }

    Rocket* createRocket(Tank* owner, const Vector2& pos, const Vector2& dir) {
      return new Rocket(owner, pos, dir, SPLIT);
    }
};

#endif /* SMARTTANK_H_ */
