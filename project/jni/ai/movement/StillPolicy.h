#ifndef STILLPOLICY_H_
#define STILLPOLICY_H_

#include "MovementPolicy.h"

class StillPolicy : public MovementPolicy {
  public:

    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
      return false;
    }
};

#endif /* STILLPOLICY_H_ */
