#ifndef SHIELDTANK_H_
#define SHIELDTANK_H_

#include "ai/shoot/AimPolicy.h"
#include "ai/shoot/NonePolicy.h"
#include "ai/movement/PathPolicy.h"
#include "ai/movement/StillPolicy.h"
#include "logic/EnemyTank.h"

class ShieldTank : public EnemyTank {
  public:
    ShieldTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    ShieldTank (Path* p)
      : EnemyTank (new TankAI(new AimPolicy(), new PathPolicy())) {
      this->setPath(p);
    }

    virtual eTankType getTankType () const { return TANK_SHIELD; }

    bool bounce (Entity* e, const Vector2& colPoint) {
      return shieldBounce(e, colPoint);
    }
};


#endif /* ORANGETANK_H_ */
