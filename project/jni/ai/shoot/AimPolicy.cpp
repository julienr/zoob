#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/Tank.h"

Vector2 AimPolicy::aim (double elapsedS, Game* game, Tank* myTank) {
  return game->getPlayerTank().getPosition() - myTank->getPosition();
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
