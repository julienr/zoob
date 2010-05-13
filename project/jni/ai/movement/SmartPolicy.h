#ifndef SMARTPOLICY_H_
#define SMARTPOLICY_H_

#include "ai/movement/MovementPolicy.h"

class SmartPolicy : public MovementPolicy {
  public:
    bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
};

#endif /* SMARTPOLICY_H_ */
