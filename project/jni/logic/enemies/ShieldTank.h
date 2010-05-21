#ifndef SHIELDTANK_H_
#define SHIELDTANK_H_

#include "ai/shoot/AimPolicy.h"
#include "ai/shoot/NonePolicy.h"
#include "ai/movement/PathPolicy.h"
#include "ai/movement/StillPolicy.h"
#include "logic/EnemyTank.h"
#include "logic/Rocket.h"

class ShieldTank : public EnemyTank {
  public:
    ShieldTank (float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    ShieldTank (Path* p, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), new PathPolicy())) {
      this->setPath(p);
    }

    virtual eTankType getTankType () const { return TANK_SHIELD; }

    virtual bool explode (Entity* e, const Vector2& colPoint) {
      if (e->getType() == ENTITY_ROCKET) {
        Rocket* r = static_cast<Rocket*>(e);
        if (r->getNumBounces() < Difficulty::getInstance()->getShieldResistance())
          return false;
      }
      return EnemyTank::explode(e, colPoint);
    }
};

#endif /* SHIELDTANK_H_ */
