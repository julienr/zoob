#ifndef AIMPOLICY_H_
#define AIMPOLICY_H_

#include "ShootPolicy.h"

class Entity;
class CollisionResult;

class AimPolicy : public ShootPolicy {
  public:
    //if followRockets, it will also aim at rockets
    AimPolicy (bool aimRockets=true) : ShootPolicy (), aimRockets(aimRockets) {}
    virtual bool decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
    virtual bool confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

    bool aim (double elapsedS, Game* game, EnemyTank* tank, Vector2* outDir);
  private:
    const bool aimRockets;
};

#endif /* AIMPOLICY_H_ */
