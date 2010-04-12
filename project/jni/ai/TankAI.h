#ifndef TANKAI_H_
#define TANKAI_H_

#include "lib/Vector2.h"
#include "movement/MovementPolicy.h"
#include "shoot/ShootPolicy.h"


/**
 * Base abstract class for a tank AI
 * all the "decideXX" functions will be called
 * every frame
 */
class TankAI {
  public:
    TankAI (ShootPolicy* sp, MovementPolicy* mp)
      : shootPolicy(sp),
        movementPolicy(mp) {
    }

    ~TankAI () {
      delete shootPolicy;
      delete movementPolicy;
    }

    bool decideFire (double elapsedS, Vector2* outDir) {
      return shootPolicy->decideFire(elapsedS, outDir);
    }

    Vector2 decideDir (double elapsedS) {
      return movementPolicy->decideDir(elapsedS);
    }
  private:
    ShootPolicy* shootPolicy;
    MovementPolicy* movementPolicy;

};

#endif
