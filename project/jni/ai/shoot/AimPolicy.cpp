#include "AimPolicy.h"

#include "logic/Game.h"
#include "logic/EnemyTank.h"

bool AimPolicy::aim (double elapsedS, Game* game, EnemyTank* myTank, Vector2* outDir) {
  //FIXME: remove 3 lines
  CollisionResult r;
  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-myTank->getPosition();
  game->getColManager().traceCircle(myTank, myTank->getPosition(), dirToTank, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET);
  *outDir = game->getPlayerTank()->getPosition() - myTank->getPosition();
  return true;
}

bool AimPolicy::decideFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* myTank) {
  CollisionResult r;
  const Vector2& tP = myTank->getPosition();

  //First, fire to approaching rockets
  Vector2 nearRocketPos;
  if (TankAI::rocketNear(game, myTank,4*GRID_CELL_SIZE, &nearRocketPos)) {
    //LOGE("rocket near");
    const Vector2 dirToRocket = nearRocketPos-tP;
    if (!game->getColManager().traceCircle(myTank, tP, dirToRocket, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)) {
      outDir->set(dirToRocket.getNormalized());
      return true;
    }
  }

  //If we reach this point,no approaching rockets, try to fire to player
  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-tP;
  if (game->getColManager().traceCircle(myTank, tP, dirToTank, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)
      && r.collidedEntity != game->getPlayerTank()) {
    //Cannot see, don't fire
    return false;
  } else {
    //LOGE("see player tank");
    outDir->set(dirToTank.getNormalized());
    return true;
  }
}

bool AimPolicy::confirmFire (double elapsedS, Vector2* outDir, Game* game, EnemyTank* tank) {
  //First, fire to approaching rockets
  Vector2 nearRocketPos;
  if (TankAI::rocketNear(game, tank,4*GRID_CELL_SIZE, &nearRocketPos)) {
    //LOGE("confirm rocket near");
    const Vector2 dirToRocket = nearRocketPos-tank->getPosition();
    CollisionResult r;
    if (!game->getColManager().traceCircle(tank, tank->getPosition(), dirToRocket, ROCKET_BCIRCLE_R, &r, ENTITY_ROCKET)) {
      outDir->set(dirToRocket.getNormalized());
      return true;
    }
  }

  const Vector2 dirToTank = game->getPlayerTank()->getPosition()-tank->getPosition();
  outDir->set(dirToTank.getNormalized());
  return true;
}
