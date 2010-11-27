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
    ShieldTank (Path* p = NULL, float radius=TANK_BCIRCLE_R)
      : EnemyTank (radius, new TankAI(new AimPolicy(), p?(MovementPolicy*)new PathPolicy():(MovementPolicy*)new StillPolicy())) {
      if (p) this->setPath(p);
    }

    virtual eTankType getTankType () const { return TANK_SHIELD; }

    int explode (Entity* e, const Vector2& colPoint) {
      if (e->getType() == ENTITY_ROCKET) {
        Rocket* r = static_cast<Rocket*>(e);
        if (r->getNumBounces() < Difficulty::getInstance()->getShieldResistance())
          return 0;
      }
      return EnemyTank::explode(e, colPoint);
    }

  protected:
    //For subclasses
    ShieldTank (float radius, TankAI* ai)
          : EnemyTank(radius, ai) {}
};

#endif /* SHIELDTANK_H_ */
