#ifndef SMARTTANK_H_
#define SMARTTANK_H_

#include "ai/movement/PathPolicy.h"
#include "ShieldTank.h"

class SmartTank: public ShieldTank {
  public:
    SmartTank (float radius=TANK_BCIRCLE_R)
      : ShieldTank (radius, new TankAI(new AimPolicy(true), new StillPolicy())) {
    }

    SmartTank (Path* p, float radius=TANK_BCIRCLE_R)
      : ShieldTank (radius, new TankAI(new AimPolicy(true), new StillPolicy())) {
      this->setPath(p);
    }

    eTankType getTankType () const { return TANK_SMART; }
};

#endif /* SMARTTANK_H_ */
