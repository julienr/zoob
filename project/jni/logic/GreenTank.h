#ifndef GREENTANK_H_
#define GREENTANK_H_

#include "ai/shoot/AimPolicy.h"
#include "ai/shoot/NonePolicy.h"
#include "ai/movement/StillPolicy.h"
#include "EnemyTank.h"

class GreenTank : public EnemyTank {
  public:
    GreenTank ()
      : EnemyTank (new TankAI(new AimPolicy(), new StillPolicy())) {
    }

    eColor getColor () const { return GREEN; }

    void explode (Entity* e, const Vector2& colPoint);
};


#endif /* GREENTANK_H_ */
