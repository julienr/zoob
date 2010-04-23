#ifndef SHIELDTANK_H_
#define SHIELDTANK_H_

#include "ai/shoot/AimPolicy.h"
#include "ai/shoot/NonePolicy.h"
#include "ai/movement/StillPolicy.h"
#include "EnemyTank.h"

class ShieldTank : public EnemyTank {
  public:
    ShieldTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    eTankType getTankType () const { return TANK_SHIELD; }

    bool bounce (Entity* e, const Vector2& colPoint);
};


#endif /* ORANGETANK_H_ */
