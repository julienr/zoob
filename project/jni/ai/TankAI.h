#ifndef TANKAI_H_
#define TANKAI_H_

#include "lib/Vector2.h"
#include "movement/MovementPolicy.h"
#include "shoot/ShootPolicy.h"


/**
 * Aggregates the different policies for one tank.
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

    bool decideFire (double elapsedS, Vector2* outDir, Game* game, Tank* tank) {
      return shootPolicy->decideFire(elapsedS, outDir, game, tank);
    }

    Vector2 decideDir (double elapsedS, Game* game, Tank* tank) {
      return movementPolicy->decideDir(elapsedS);
    }

    Vector2 aim (double elapsedS, Game* game, Tank* tank) {
      return shootPolicy->aim(elapsedS, game, tank);
    }
  private:
    ShootPolicy* shootPolicy;
    MovementPolicy* movementPolicy;

};

#endif
