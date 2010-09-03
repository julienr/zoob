#ifndef STILLPOLICY_H_
#define STILLPOLICY_H_

#include "MovementPolicy.h"

class StillPolicy : public MovementPolicy {
  public:

    bool decideDir (double UNUSED(elapsedS), Vector2* UNUSED(outDir), Game* UNUSED(game), EnemyTank* UNUSED(tank)) {
      return false;
    }
};

#endif /* STILLPOLICY_H_ */
