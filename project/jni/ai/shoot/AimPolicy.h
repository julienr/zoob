#ifndef AIMPOLICY_H_
#define AIMPOLICY_H_

#include "ShootPolicy.h"

class AimPolicy : public ShootPolicy {
  public:
    bool decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
    bool confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

    bool aim (double elapsedS, Game* game, EnemyTank* tank, Vector2* outDir);
};

#endif /* AIMPOLICY_H_ */
