#ifndef SHOOT_POLICY_H_
#define SHOOT_POLICY_H_

#include "def.h"
#include "lib/Vector2.h"

class Game;
class Tank;

class ShootPolicy {
  public:
    //Should return true if the tank decide to fire a rocket
    virtual bool decideFire (double elapsedS, Vector2* outDir, Game* game, Tank* tank) = 0;

    //Should return the tank rotation
    virtual Vector2 aim (double elapsedS, Game* game, Tank* tank) = 0;
};

#endif /* SHOOT_POLICY_H_ */
