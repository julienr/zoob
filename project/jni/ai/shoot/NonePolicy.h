#ifndef NONEPOLICY_H_
#define NONEPOLICY_H_

#include "ShootPolicy.h"

class NonePolicy : public ShootPolicy {
  public:
    bool decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
      return false;
    }

    bool aim (double elapsedS, Game* game, EnemyTank* tank, Vector2* outDir) {
      return false;
    }
};

#endif /* NONEPOLICY_H_ */
