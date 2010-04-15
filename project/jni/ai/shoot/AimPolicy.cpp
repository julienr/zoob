#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/Tank.h"

bool AimPolicy::aim (double elapsedS, Game* game, Tank* myTank, Vector2* outDir) {
  *outDir = game->getPlayerTank().getPosition() - myTank->getPosition();
  return true;
}

bool AimPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, Tank* myTank) {
  CollisionResult r;
  const Vector2 tP = myTank->getPosition();
  const Vector2 dirToTank = game->getPlayerTank().getPosition()-tP;
  if (game->getColManager().traceRay(myTank, tP, dirToTank, &r)
      && r.collidedEntity != &game->getPlayerTank()) {
    //LOGE("entity : %p, tFirst : %f, myTank : %p", r.collidedEntity, r.tFirst, myTank);
    //Cannot see, don't fire
    return false;
  } else {
    LOGE("see player tank");
    outDir->set(dirToTank.getNormalized());
    return true;
  }
}
