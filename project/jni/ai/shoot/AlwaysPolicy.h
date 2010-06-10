#ifndef ALWAYSPOLICY_H_
#define ALWAYSPOLICY_H_

#include "AimPolicy.h"

class Entity;

class AlwaysPolicy : public AimPolicy {
  public:
    AlwaysPolicy () : AimPolicy() {}
    bool decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
    bool confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
};

#endif /* ALWAYSPOLICY_H_ */
