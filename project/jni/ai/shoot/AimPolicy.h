#ifndef AIMPOLICY_H_
#define AIMPOLICY_H_

#include "ShootPolicy.h"

class Entity;
class CollisionResult;

class AimPolicy : public ShootPolicy {
  public:
    AimPolicy (bool useBounceTrace=false) : ShootPolicy (), useBounceTrace(useBounceTrace) {}
    bool decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);
    bool confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank);

    bool aim (double elapsedS, Game* game, EnemyTank* tank, Vector2* outDir);
  private:
    bool doTraceCircle (Game* game, Entity* source, const Vector2& start, const Vector2& move, CollisionResult* result);
    const bool useBounceTrace;
};

#endif /* AIMPOLICY_H_ */
