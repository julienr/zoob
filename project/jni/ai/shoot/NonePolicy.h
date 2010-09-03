#ifndef NONEPOLICY_H_
#define NONEPOLICY_H_

#include "ShootPolicy.h"

class NonePolicy : public ShootPolicy {
  public:
    bool decideFire (double UNUSED(elapsedS), Vector2* UNUSED(outDir), Game* UNUSED(game), EnemyTank* UNUSED(tank)) {
      return false;
    }

    bool aim (double UNUSED(elapsedS), Game* UNUSED(game), EnemyTank* UNUSED(tank), Vector2* UNUSED(outDir)) {
      return false;
    }
};

#endif /* NONEPOLICY_H_ */
