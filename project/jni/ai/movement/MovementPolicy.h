#ifndef MOVEMENTPOLICY_H_
#define MOVEMENTPOLICY_H_

#include "def.h"
#include "lib/Vector2.h"

class Game;
class EnemyTank;

class MovementPolicy {
  public:
    virtual bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) = 0;
    virtual bool requirePlayerVisibility () { return false; }
};

#endif /* MOVEMENTPOLICY_H_ */
