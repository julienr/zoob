#ifndef AIMPOLICY_H
#define AIMPOLICY_H_

#include "ShootPolicy.h"

class AimPolicy : public ShootPolicy {
  public:
    bool decideFire (double elapsedS, Vector2* outDir) {
      return false;
    }

    Vector2 aim (double elapsedS, Game* game, Tank* tank);
};

#endif /* AIMPOLICY_H_ */
