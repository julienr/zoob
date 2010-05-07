#ifndef MOVEMENTPOLICY_H_
#define MOVEMENTPOLICY_H_

#include "def.h"
#include "lib/Vector2.h"

class Game;
class EnemyTank;

class MovementPolicy {
  public:
    virtual bool decideDir (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) = 0;

  protected:
    //Utility function to determine if a rocket is near this tank. The given radius is the radius of
    //the "near" area
    bool rocketNear (Game* game, EnemyTank* tank, float radius);
};

#endif /* MOVEMENTPOLICY_H_ */
