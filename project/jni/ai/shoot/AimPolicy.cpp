#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AimPolicy::aim (double elapsedS, Game* game, EnemyTank* myTank, Vector2* outDir) {
  *outDir = game->getPlayerTank().getPosition() - myTank->getPosition();
  return true;
}

bool AimPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* myTank) {
  CollisionResult r;
  const Vector2 tP = myTank->getPosition();
  const Vector2 dirToTank = game->getPlayerTank().getPosition()-tP;

  if (game->getColManager().traceCircle(myTank, tP, dirToTank, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)
      && r.collidedEntity != &game->getPlayerTank()) {
    //LOGE("entity : %p, tFirst : %f, myTank : %p", r.collidedEntity, r.tFirst, myTank);

    //Cannot see, don't fire
    return false;
  } else {
    //LOGE("see player tank");
    outDir->set(dirToTank.getNormalized());
    return true;
  }
}

bool AimPolicy::confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  const Vector2 dirToTank = game->getPlayerTank().getPosition()-tank->getPosition();
  outDir->set(dirToTank.getNormalized());
  return true;
}
