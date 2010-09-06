#ifndef BOUNCETANK_H_
#define BOUNCETANK_H_

#include "ai/shoot/AimPolicy.h"
#include "ai/shoot/NonePolicy.h"
#include "ai/movement/PathPolicy.h"
#include "ai/movement/StillPolicy.h"
#include "logic/EnemyTank.h"

class BounceTank : public EnemyTank {
  public:
    BounceTank (Path*p = NULL, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(false), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new StillPolicy())) {
      if (p) this->setPath(p);
    }

    virtual eTankType getTankType () const { return TANK_BOUNCE; }

    bool bounce (Entity* e, const Vector2& colPoint) {
      return shieldBounce(e, colPoint);
    }
};


#endif /* BOUNCETANK_H_ */
